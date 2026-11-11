/* SPDX-License-Identifier: MIT */
/** @file kymo_memory.h @brief Memory Low-Level benchmark only; intentionally no Endpoint. */
#pragma once
#include <stdint.h>
/** @brief Measure the feature operation. @return CPU cycles for RUNS iterations. */
uint32_t benchMemory(void);
/** @brief Initialize persistent memory benchmark state. */
void benchmarkMemoryInit(void);
