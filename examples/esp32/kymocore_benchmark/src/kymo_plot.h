/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief Plot Low-Level benchmark only; intentionally no Endpoint. */
#pragma once
#include <stdint.h>
/** @brief Measure the feature operation. @return CPU cycles for RUNS iterations. */
uint32_t benchEncode(void);
