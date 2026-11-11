/* SPDX-License-Identifier: MIT */
/** @file native_port.cpp
 * @brief Host-only port driving the actual Arduino setup/loop application.
 */
#include "kymo_init.h"
#include "kymo_port.h"
#include "app_values.h"
#include "features/kymo_watch.h"
#include <common/embedded_bytes.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void setup(void);
void loop(void);

static const size_t INPUT_CAPACITY = 128u;
static const size_t IO_CHUNK_BYTES = 3u;
static const uint32_t STALL_EVERY_CALLS = 5u;
static const uint32_t RUN_DURATION_MS = 5000u;
static const uint32_t DRAIN_LIMIT = 10000u;
static const float EXPECTED_GAIN = 2.5f;
static const float INITIAL_GAIN = 1.0f;
static const float FLOAT_TOLERANCE = 0.0001f;
static uint8_t sInput[INPUT_CAPACITY];
static size_t sInputLength;
static size_t sInputOffset;
static uint32_t sNowMs;
static uint32_t sWriteCalls;
static FILE *sOutput;

/** @brief Adapt the final WATCH_SET request to this process's ELF address.
 * @details The runner supplies the independently encoded request fixture.
 */
/*******************************************************************************
 * bindWatchAddress
 ******************************************************************************/
static void bindWatchAddress(void)
{
    const size_t controlHeaderBytes = 4u;
    const size_t watchPayloadBytes = 10u;
    const size_t crcBytes = 1u;
    const size_t addressPayloadOffset = 3u;
    const size_t syncBytes = 2u;
    const uint8_t crcPolynomial = 0x07u;
    const uint8_t highBit = 0x80u;
    const uint8_t bitsPerByte = 8u;
    const KymoEndpointWatchConfig config = kymoWatchConfig();
    uint8_t crc = 0u;
    uint8_t *frame = nullptr;
    assert(sInputLength >= controlHeaderBytes + watchPayloadBytes + crcBytes);
    frame = sInput + sInputLength - controlHeaderBytes - watchPayloadBytes - crcBytes;
    assert(frame[controlHeaderBytes] == KYMO_WATCH_OP_SET);
    emb_write_u32_le(frame + controlHeaderBytes + addressPayloadOffset, config.watch.ranges[0].start);
    for (size_t index = syncBytes; index < controlHeaderBytes + watchPayloadBytes; ++index) {
        crc ^= frame[index];
        for (uint8_t bit = 0u; bit < bitsPerByte; ++bit) {
            crc = static_cast<uint8_t>((crc << 1u) ^ ((crc & highBit) != 0u ? crcPolynomial : 0u));
        }
    }
    frame[controlHeaderBytes + watchPayloadBytes] = crc;
}

/** @brief Host port initialization; the runner owns file handles. */
/*******************************************************************************
 * kymoPortInit
 ******************************************************************************/
void kymoPortInit(void) {}

/** @brief Return simulated milliseconds. @param user Unused. @return Time. */
/*******************************************************************************
 * kymoPortClockMs
 ******************************************************************************/
uint32_t kymoPortClockMs(void *user)
{
    (void)user;
    return sNowMs;
}

/** @brief Return simulated trace ticks. @param user Unused. @return Time. */
/*******************************************************************************
 * kymoPortClockTicks
 ******************************************************************************/
uint64_t kymoPortClockTicks(void *user)
{
    (void)user;
    return sNowMs;
}

/** @brief Fragment control input. @param user Unused.
 * @param bytes Destination. @param capacity Maximum count. @return Copied bytes. */
/*******************************************************************************
 * kymoPortRead
 ******************************************************************************/
size_t kymoPortRead(void *user, uint8_t *bytes, size_t capacity)
{
    size_t count = sInputLength - sInputOffset;
    (void)user;
    assert(bytes != nullptr);
    if (count > capacity) { count = capacity; }
    if (count > IO_CHUNK_BYTES) { count = IO_CHUNK_BYTES; }
    memcpy(bytes, sInput + sInputOffset, count);
    sInputOffset += count;
    return count;
}

/** @brief Copy partial TX with periodic backpressure. @param user Unused.
 * @param bytes Frame prefix. @param length Offered count. @return Accepted bytes. */
/*******************************************************************************
 * kymoPortWrite
 ******************************************************************************/
size_t kymoPortWrite(void *user, const uint8_t *bytes, size_t length)
{
    size_t count = length > IO_CHUNK_BYTES ? IO_CHUNK_BYTES : length;
    (void)user;
    assert(bytes != nullptr);
    ++sWriteCalls;
    if ((sWriteCalls % STALL_EVERY_CALLS) == 0u) { count = 0u; }
    assert(fwrite(bytes, 1u, count, sOutput) == count);
    return count;
}

/** @brief Execute real application and drain its final trace session.
 * @param argc Argument count. @param argv Input and capture paths. @return Zero. */
/*******************************************************************************
 * main
 ******************************************************************************/
int main(int argc, char **argv)
{
    FILE *input = nullptr;
    KymoStatus status = KYMO_BAD_CONFIG;
    assert(argc == 3);
    input = fopen(argv[1], "rb");
    assert(input != nullptr);
    sInputLength = fread(sInput, 1u, sizeof(sInput), input);
    assert(ferror(input) == 0 && sInputLength < sizeof(sInput));
    assert(fclose(input) == 0);
    if (sInputLength != 0u) { bindWatchAddress(); }
    sOutput = fopen(argv[2], "wb");
    assert(sOutput != nullptr);
    setup();
    for (sNowMs = 0u; sNowMs < RUN_DURATION_MS; ++sNowMs) { loop(); }
    assert(sInputOffset == sInputLength);
    assert(fabsf(appGain - (sInputLength != 0u ? EXPECTED_GAIN : INITIAL_GAIN)) < FLOAT_TOLERANCE);
    status = KymoApp_RequestStop();
    assert(status == KYMO_OK);
    for (uint32_t cycle = 0u; cycle < DRAIN_LIMIT; ++cycle) {
        status = KymoApp_Main();
        assert(status == KYMO_OK || status == KYMO_BUSY || status == KYMO_IDLE);
        if (status == KYMO_IDLE) { break; }
        ++sNowMs;
    }
    assert(status == KYMO_IDLE);
    assert(fclose(sOutput) == 0);
    return 0;
}
