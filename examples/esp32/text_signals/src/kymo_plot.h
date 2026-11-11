/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief Text-only diagnostic waveform generation. */
#pragma once
#include <stdint.h>
/** @brief Coherent pair of dimensionless values. */
struct TextSamples { float sine; /**< 1-Hz sine. */ float saw; /**< 0.5-Hz sawtooth. */ };
/** @brief Compute one sample pair. @param[in] index 100-Hz grid index. @return Values. */
TextSamples textPlotSample(uint32_t index);
