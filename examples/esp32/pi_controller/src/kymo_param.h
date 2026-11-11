/* SPDX-License-Identifier: MIT */
/** @file kymo_param.h @brief Param bindings and configuration for this example.
 * @details Referenced tables persist; invoke Init once before Endpoint Main.
 * @par Usage Pass the configuration to the matching KymoEndpointParam_Init. */
#pragma once
#include <endpoint/features/kymo_param.h>

/** @brief Build this feature's configuration.
 * @return Config with persistent application bindings. */
KymoEndpointParamConfig kymoParamConfig(void);
/** @brief Initialize this feature after base Init.
 * @param[in,out] endpoint Base. @param[in] config Feature settings. @return Init status. */
KymoStatus kymoParamInit(KymoEndpoint *endpoint, const KymoEndpointParamConfig *config);
