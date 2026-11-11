/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief Plot bindings and configuration for this example.
 * @details Referenced tables persist; invoke Init once before Endpoint Main.
 * @par Usage Pass the configuration to the matching KymoEndpointPlot_Init. */
#pragma once
#include <endpoint/features/kymo_plot.h>

/** @brief Build this feature's configuration.
 * @return Config with persistent application bindings. */
KymoEndpointPlotConfig kymoPlotConfig(void);
/** @brief Logical grid clock, shared by all channels of one set.
 * @param[in] user Unused. @return Grid time in milliseconds. */
uint32_t kymoPlotClock(void *user);
