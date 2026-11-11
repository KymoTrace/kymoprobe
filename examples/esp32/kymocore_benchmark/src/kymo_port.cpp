/* SPDX-License-Identifier: MIT */
/** @file kymo_port.cpp @brief Arduino text report adapter. */
#include <Arduino.h>
#include "kymo_port.h"
#include "benchmark.h"
/*******************************************************************************
 * benchmarkReport
 ******************************************************************************/
void benchmarkReport(const char *name, uint32_t cycles)
{
    const float perRun = static_cast<float>(cycles) / static_cast<float>(RUNS);
    const float micros = perRun / static_cast<float>(getCpuFrequencyMhz());
    Serial.printf("%-22s %8.1f cycles %7.2f us\n", name, perRun, micros);
}
