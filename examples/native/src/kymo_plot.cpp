/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Portable channel and constant sample. */
#include "kymo_plot.h"
/*******************************************************************************
 * readSample
 ******************************************************************************/
static uint8_t readSample(void *user, uint8_t id, KymoSample *sample)
{
    uint8_t valid = 0u;
    (void)user;
    if ((sample != nullptr) && (id == 7u)) { sample->value = 1.25f; valid = 1u; }
    return valid;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    static const KymoChannel channels[] = {{20u, 7u, 0u}};
    KymoEndpointPlotConfig config = {};
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = readSample;
    return config;
}
