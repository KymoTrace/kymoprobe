/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Portable scalar variable binding through Endpoint Plot. */
#include "kymo_plot.h"

static const float value = 1.25f;
static const KymoChannel channels[] = {{20u, 7u, 0u}};
static const float *const values[] = {&value};
static_assert(KYMO_COUNT_OF(values) == KYMO_COUNT_OF(channels), "one variable per channel");
static KymoPlotBinding binding = {channels, values, KYMO_COUNT_OF(channels)};

/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = Kymo_SampleBound;
    config.sample_user = &binding;
    return config;
}
