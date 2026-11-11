/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Plot bindings from immutable configurator data. */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_DEMO_MODE == KYMO_DEMO_GENERATED && KYMO_EFFECTIVE_RUNTIME
#include "kymo_plot.h"
#include <kymo_generated_config.h>
/*******************************************************************************
 * generatedPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig generatedPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    config.channels = kymo_generated_config.channels;
    config.channel_count = kymo_generated_config.channel_count;
    config.sample = kymo_generated_config.sample;
    config.sample_user = kymo_generated_config.sample_user;
    return config;
}
#endif
