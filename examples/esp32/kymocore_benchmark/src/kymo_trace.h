/* SPDX-License-Identifier: MIT */
/** @file kymo_trace.h @brief Trace Low-Level benchmark only; intentionally no Endpoint. */
#pragma once
#include <stdint.h>
/** @brief Measure the feature operation. @return CPU cycles for RUNS iterations. */
uint32_t benchTrace(void);
/** @brief Measure the feature operation. @return CPU cycles for RUNS iterations. */
uint32_t benchCrc16(void);
/** @brief Initialize persistent trace benchmark state. */
void benchmarkTraceInit(void);
