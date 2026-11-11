/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h @brief Native test clock and copying capture port. */
#pragma once
#include <endpoint/kymo_endpoint.h>
/** @brief Simulated clock advanced by the application. */
extern uint32_t nativeTickMs;
/** @brief Valid frames accepted by the port. */
extern uint32_t nativeFrameCount;
/** @brief Build the simulated port. @return Callbacks without buffers. */
KymoEndpointConfig kymoPortConfig(void);
