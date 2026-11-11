/* SPDX-License-Identifier: MIT */
/** @file measurement_signals_test.cpp @brief Actual Endpoint waveform binding test. */
#include <stdio.h>
#include <assert.h>
#include <endpoint/kymo_endpoint.h>
#include <endpoint/features/kymo_plot.h>
#include "kymo_plot.h"
#include "Config.h"
static uint32_t clockMs;
static FILE *output;
static bool blocked;
/*******************************************************************************
 * probeSerialClock
 ******************************************************************************/
uint32_t probeSerialClock(void *user)
{
    (void)user;
    return clockMs;
}
/*******************************************************************************
 * writeBytes
 ******************************************************************************/
static size_t writeBytes(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    const size_t CHUNK_BYTES = 3u;
    (void)user;
    if (!blocked) { accepted = fwrite(bytes, 1u, length < CHUNK_BYTES ? length : CHUNK_BYTES, output); }
    return accepted;
}
/*******************************************************************************
 * main
 ******************************************************************************/
int main(int argc, char **argv)
{
    KymoEndpoint endpoint = {};
    KymoEndpointPlot plot = {};
    KymoEndpointConfig port = {};
    uint8_t buffer[KYMO_FRAME_MAX_SIZE] = {};
    assert(argc >= 2);
    output = fopen(argv[1], "wb");
    assert(output != nullptr);
    const KymoEndpointPlotConfig config = kymoPlotConfig();
    port.clock_ms = kymoPlotClock;
    port.tx.buffer = buffer;
    port.tx.capacity = sizeof(buffer);
    port.tx.write = writeBytes;
    assert(KymoEndpoint_Init(&endpoint, &port) == KYMO_OK);
    assert(KymoEndpointPlot_Init(&endpoint, &plot, &config) == KYMO_OK);
    for (uint32_t index = 0u; index < 2048u; ++index) {
        clockMs = index * SAMPLE_PERIOD_MS;
        blocked = argc > 2 && index >= 512u && index < 562u;
        for (uint32_t turn = 0u; turn < 80u; ++turn) {
            (void)KymoEndpoint_Main(&endpoint);
        }
    }
    assert(KymoEndpoint_RequestStop(&endpoint) == KYMO_OK);
    for (uint32_t turn = 0u; turn < 1000u && endpoint.phase != KYMO_ENDPOINT_STOPPED; ++turn) {
        (void)KymoEndpoint_Main(&endpoint);
    }
    assert(endpoint.phase == KYMO_ENDPOINT_STOPPED);
    assert(fclose(output) == 0);
    return 0;
}
