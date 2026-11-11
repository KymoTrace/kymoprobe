/* SPDX-License-Identifier: MIT */
/** @file benchmark.h @brief Shared benchmark repetition count and observable accumulator. */
#pragma once
#include <stdint.h>
/** @brief Repetitions per measurement. */
static const uint32_t RUNS = 1000u;
/** @brief Volatile accumulator preventing dead-code elimination. */
extern volatile uint32_t sink;
