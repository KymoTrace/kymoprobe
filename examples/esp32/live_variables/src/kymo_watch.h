/* SPDX-License-Identifier: MIT */
/** @file kymo_watch.h @brief Watch bindings and configuration for this example.
 * @details Referenced tables persist; invoke Init once before Endpoint Main.
 * @par Usage Pass the configuration to the matching KymoEndpointWatch_Init. */
#pragma once
#include <endpoint/features/kymo_watch.h>

/** @brief Build this feature's configuration.
 * @return Config with persistent application bindings. */
KymoEndpointWatchConfig kymoWatchConfig(void);
/** @brief Initialize this feature after base Init.
 * @param[in,out] endpoint Base. @param[in] config Feature settings. @return Init status. */
KymoStatus kymoWatchInit(KymoEndpoint *endpoint, const KymoEndpointWatchConfig *config);
