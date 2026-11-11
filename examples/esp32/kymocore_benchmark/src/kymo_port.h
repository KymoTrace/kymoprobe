/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h @brief Text benchmark result output, separate from timed operations. */
#pragma once
#include <stdint.h>
/** @brief Print mean cost. @param[in] name Operation. @param[in] cycles Total CPU cycles. */
void benchmarkReport(const char *name, uint32_t cycles);
