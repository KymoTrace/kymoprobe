/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h @brief Arduino Serial port, separated from feature bindings.
 * @details Serial is exclusively binary; callbacks copy data and never retain it.
 * @par Usage Start Serial in the application, then pass probeSerialConfig to Init. */
#pragma once
#include <endpoint/kymo_endpoint.h>
#if KYMO_EFFECTIVE_ENDPOINT
/** @brief Read the port millisecond clock. @param[in] user Unused.
 * @return Milliseconds modulo 2^32. */
uint32_t probeSerialClock(void *user);
/** @brief Construct bounded serial I/O and millisecond clock callbacks.
 * @return Port-only config, without buffers or features.
 * @par Usage The application owns baud rate and SDK setup. */
KymoEndpointConfig probeSerialConfig(void);
#endif
