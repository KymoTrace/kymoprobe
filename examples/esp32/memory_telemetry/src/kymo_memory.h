/* SPDX-License-Identifier: MIT */
/** @file kymo_memory.h @brief Memory bindings and configuration for this example.
 * @details Referenced tables persist; invoke Init once before Endpoint Main.
 * @par Usage Pass the configuration to the matching KymoEndpointMemory_Init. */
#pragma once
#include <endpoint/features/kymo_memory.h>

/** @brief Build this feature's configuration.
 * @return Config with persistent application bindings. */
KymoEndpointMemoryConfig kymoMemoryConfig(void);
/** @brief Initialize this feature after base Init.
 * @param[in,out] endpoint Base. @param[in] config Feature settings. @return Init status. */
KymoStatus kymoMemoryInit(KymoEndpoint *endpoint, const KymoEndpointMemoryConfig *config);
