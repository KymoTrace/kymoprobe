/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief CPU cost of KymoCore operations on the ESP32 (cycle counter).
 * @details Every 5 s each operation runs RUNS times; the mean cost in cycles and
 * microseconds goes out as one text line per operation. Measured: encoding a DATA
 * frame with timestamp and CRC-8, one trace ENTER/EXIT pair, the trace CRC-16 over a
 * full 240-byte frame and one KymoMemory sample. The two PlatformIO environments differ
 * only in KYMO_ENABLE_CRC_TABLE. Interrupts stay enabled, so single outliers are
 * possible; the mean over RUNS smooths them. */
#include <Arduino.h>
#include <KymoCore.h>
#include <xtensa/core-macros.h>

static const uint32_t BENCH_BAUD = 115200u;
static const uint32_t RUNS = 1000u;
static const uint32_t REPORT_PERIOD_MS = 5000u;
static const size_t TRACE_FRAME_BYTES = 240u;

static KymoTraceRecorder recorder;
static KymoMemory memory;
static const KymoMemoryEntry entries[1] = {{"bench", 4096u, KYMO_MEMORY_STACK_PEAK, 200u}};
static uint8_t traceFrame[TRACE_FRAME_BYTES];
static volatile uint32_t sink = 0u;
static uint32_t lastReportMs = 0u;

/** @brief Trace clock: the cycle counter. @param[in] user Unused. @return Cycles. */
/*******************************************************************************
 * traceTicks
 ******************************************************************************/
static uint64_t traceTicks(void *user)
{
    (void)user;
    return static_cast<uint64_t>(XTHAL_GET_CCOUNT());
}

/** @brief Memory reader: a constant value, so only KymoMemory itself is measured.
 * @param[in] context Unused. @param[in] index Unused. @param[out] bytes Value. @return 1. */
/*******************************************************************************
 * readMemory
 ******************************************************************************/
static uint8_t readMemory(void *context, size_t index, uint32_t *bytes)
{
    (void)context;
    (void)index;
    *bytes = 1234u;
    return 1u;
}

/** @brief Print the mean cost of one operation.
 * @param[in] name Operation. @param[in] cycles Cycles for RUNS repetitions. */
/*******************************************************************************
 * report
 ******************************************************************************/
static void report(const char *name, uint32_t cycles)
{
    const float perRun = static_cast<float>(cycles) / static_cast<float>(RUNS);
    const float micros = perRun / static_cast<float>(getCpuFrequencyMhz());
    Serial.printf("%-22s %8.1f cycles %7.2f us\n", name, perRun, micros);
}

/** @brief Encode RUNS DATA frames with timestamp and CRC-8. @return Cycles. */
/*******************************************************************************
 * benchEncode
 ******************************************************************************/
static uint32_t benchEncode(void)
{
    uint8_t frame[KYMO_FRAME_MAX_SIZE] = {};
    KymoDataPoint point = {};
    point.id = 1u;
    point.flags = KYMO_FLAG_TIMESTAMP;
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        point.value = static_cast<float>(run);
        point.timestamp_ms = run;
        sink += static_cast<uint32_t>(KymoProtocol_Encode(&point, frame, sizeof(frame)));
    }
    return XTHAL_GET_CCOUNT() - start;
}

/** @brief Record RUNS ENTER/EXIT pairs and drop them again. @return Cycles of the recording. */
/*******************************************************************************
 * benchTrace
 ******************************************************************************/
static uint32_t benchTrace(void)
{
    KymoTraceEvent event;
    uint32_t cycles = 0u;
    for (uint32_t run = 0u; run < RUNS; ++run) {
        const uint32_t start = XTHAL_GET_CCOUNT();
        KymoTrace_Enter(&recorder, 1u, 1u);
        KymoTrace_Exit(&recorder, 1u, 1u);
        cycles += XTHAL_GET_CCOUNT() - start;
        while (KymoTrace_Pop(&recorder, &event) == KYMO_OK) {
            sink += event.depth;
        }
    }
    return cycles;
}

/** @brief CRC-16 over a full trace frame RUNS times. @return Cycles. */
/*******************************************************************************
 * benchCrc16
 ******************************************************************************/
static uint32_t benchCrc16(void)
{
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        traceFrame[0] = static_cast<uint8_t>(run);
        sink += KymoTrace_Crc16(traceFrame, sizeof(traceFrame));
    }
    return XTHAL_GET_CCOUNT() - start;
}

/** @brief RUNS KymoMemory samples (a new round each time). @return Cycles. */
/*******************************************************************************
 * benchMemory
 ******************************************************************************/
static uint32_t benchMemory(void)
{
    KymoDataPoint point = {};
    uint32_t nowMs = 0u;
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        nowMs += 1u;
        if (KymoMemory_NextSample(&memory, nowMs, &point) == KYMO_OK) {
            sink += point.id;
        }
    }
    return XTHAL_GET_CCOUNT() - start;
}

/** @brief Start the UART, the recorder and KymoMemory. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    KymoTraceConfig traceConfig = {};
    Serial.begin(BENCH_BAUD);
    traceConfig.clock_ticks = traceTicks;
    traceConfig.session_id = 1u;
    traceConfig.clock_domain_id = 1u;
    (void)KymoTrace_Init(&recorder, &traceConfig);
    const KymoMemoryConfig memoryConfig = {entries, 1u, 1u, 0u, readMemory, nullptr};
    (void)KymoMemory_Init(&memory, &memoryConfig);
    for (size_t index = 0u; index < sizeof(traceFrame); ++index) {
        traceFrame[index] = static_cast<uint8_t>(index * 7u);
    }
    lastReportMs = millis();
}

/** @brief Run the measurements every REPORT_PERIOD_MS and print one line each. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    if ((millis() - lastReportMs) >= REPORT_PERIOD_MS) {
        lastReportMs += REPORT_PERIOD_MS;
        Serial.printf("KymoCore benchmark, %u MHz, CRC tables %s, %u runs each\n",
                      static_cast<unsigned>(getCpuFrequencyMhz()), KYMO_ENABLE_CRC_TABLE ? "on" : "off",
                      static_cast<unsigned>(RUNS));
        report("DATA frame encode+CRC8", benchEncode());
        report("trace ENTER+EXIT", benchTrace());
        report("trace CRC16 240 B", benchCrc16());
        report("memory sample", benchMemory());
    }
    vTaskDelay(pdMS_TO_TICKS(10u));
}
