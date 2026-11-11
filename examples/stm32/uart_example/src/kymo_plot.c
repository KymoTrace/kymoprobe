/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.c @brief C-compatible waveform bindings. */
#include "board.h"
#include "kymo_plot.h"
#include <math.h>
/*******************************************************************************
 * sample
 ******************************************************************************/
static uint8_t sample(void *user, uint8_t id, KymoSample *out) {
    float phase = (HAL_GetTick() % 2000u) / 2000.0f;
    (void)user;
    out->value = id == 0 ? sinf(phase * 6.283185307f) : 2.0f * phase - 1.0f;
    return 1;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    static const KymoChannel channels[] = {{20u, 0u, KYMO_FLAG_TIMESTAMP}, {20u, 1u, KYMO_FLAG_TIMESTAMP}};
    KymoEndpointPlotConfig config = {0};
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = sample;
    return config;
}
