/* SPDX-License-Identifier: MIT */
/** @file main.cpp @brief Schedule diagnostics; Low-Level timing purpose intentionally preserved. */
#include <Arduino.h>
#include <KymoCore.h>
#include "benchmark.h"
#include "kymo_plot.h"
#include "kymo_trace.h"
#include "kymo_memory.h"
#include "kymo_port.h"
static const uint32_t BENCH_BAUD = 115200u;
static const uint32_t REPORT_PERIOD_MS = 5000u;
static uint32_t lastReportMs;
volatile uint32_t sink;
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.begin(BENCH_BAUD);
    benchmarkTraceInit();
    benchmarkMemoryInit();
    lastReportMs = millis();
}
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
        benchmarkReport("DATA frame encode+CRC8", benchEncode());
        benchmarkReport("trace ENTER+EXIT", benchTrace());
        benchmarkReport("trace CRC16 240 B", benchCrc16());
        benchmarkReport("memory sample", benchMemory());
    }
    vTaskDelay(pdMS_TO_TICKS(10u));
}
