/* SPDX-License-Identifier: MIT */
/** @file kymo_init.cpp @brief Common base resources; no feature configuration. */
#include "probe/kymo_init.h"
#if KYMO_EFFECTIVE_ENDPOINT
/*******************************************************************************
 * probeCoreInit
 ******************************************************************************/
KymoStatus probeCoreInit(ProbeCore *core, const KymoEndpointConfig *port)
{
    KymoStatus status = KYMO_BAD_CONFIG;
    KymoEndpointConfig config = {};
    if ((core != nullptr) && (port != nullptr)) {
        config = *port;
        config.tx.buffer = core->tx;
        config.tx.capacity = sizeof(core->tx);
        if (config.read != nullptr) {
            config.rx_buffer = core->rx;
            config.rx_capacity = sizeof(core->rx);
        }
        status = KymoEndpoint_Init(&core->endpoint, &config);
    }
    return status;
}
#endif
