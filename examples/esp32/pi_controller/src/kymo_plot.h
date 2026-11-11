/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief Plot bindings and configuration for this example.
 * @details Referenced tables persist; invoke Init once before Endpoint Main.
 * @par Usage Pass the configuration to the matching KymoEndpointPlot_Init. */
#pragma once
#include <endpoint/features/kymo_plot.h>

/** @brief Build this feature's configuration.
 * @return Config with persistent application bindings. */
KymoEndpointPlotConfig kymoPlotConfig(void);
/** @brief Initialize this feature after base Init.
 * @param[in,out] endpoint Base. @param[in] config Feature settings. @return Init status. */
KymoStatus kymoPlotInit(KymoEndpoint *endpoint, const KymoEndpointPlotConfig *config);
