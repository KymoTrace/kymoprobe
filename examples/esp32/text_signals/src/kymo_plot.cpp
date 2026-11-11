/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Waveform feature for reception-time diagnostics. */
#include "kymo_plot.h"
#include <math.h>
/*******************************************************************************
 * textPlotSample
 ******************************************************************************/
TextSamples textPlotSample(uint32_t index)
{
    static const float TWO_PI = 6.283185307f;
    TextSamples sample = {};
    sample.sine = sinf(static_cast<float>(index % 100u) / 100.0f * TWO_PI);
    sample.saw = static_cast<float>(index % 200u) / 100.0f - 1.0f;
    return sample;
}
