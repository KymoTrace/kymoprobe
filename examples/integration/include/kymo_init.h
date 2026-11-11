/* SPDX-License-Identifier: MIT */
/** @file kymo_init.h
 * @brief Common resources and processing; no feature configurations here.
 * @details Static contexts; call from one superloop, without concurrent calls.
 * @par Usage Base Init, selected feature Init, then Main cyclically.
 */
#ifndef EXAMPLE_KYMO_INIT_H
#define EXAMPLE_KYMO_INIT_H
#include <endpoint/kymo_endpoint.h>
/** @brief Prepare the project's persistent transport and clock resources.
 * @return Base configuration. @par Usage Pass to KymoApp_Init. */
KymoEndpointConfig kymoCoreConfig(void);
/** @brief Initialize the common Endpoint once.
 * @param[in] config Base resources; referenced storage must persist.
 * @return Library initialization status. @par Usage Before feature Init. */
KymoStatus KymoApp_Init(const KymoEndpointConfig *config);
/** @brief Process the selected features and shared transport once.
 * @return OK, IDLE, BUSY, recoverable BAD_SAMPLE, or a terminal error.
 * @par Usage The single cyclic Kymo call. */
KymoStatus KymoApp_Main(void);
/** @brief Stop new RX/samples and drain through the same Main function.
 * @details Stop external Trace producers first; no borrowed buffer is discarded.
 * @return Endpoint stop status. @par Usage Optional; then Main until IDLE. */
KymoStatus KymoApp_RequestStop(void);
#endif
