/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief MQTT waveform channels and sample callback. */
#include "kymo_plot.h"
#include <Arduino.h>
#include <math.h>
/*******************************************************************************
 * sample
 ******************************************************************************/
static uint8_t sample(void *, uint8_t id, KymoSample *out) {
    float phase = (millis() % 2000u) / 2000.0f;
    out->value = id == 0 ? sinf(phase * 6.283185307f) : 2.0f * phase - 1.0f;
    return 1;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    static const KymoChannel channels[] = {{100u, 0u, KYMO_FLAG_TIMESTAMP}, {100u, 1u, KYMO_FLAG_TIMESTAMP}};
    KymoEndpointPlotConfig config = {};
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = sample;
    return config;
}
