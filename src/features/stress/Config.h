/* SPDX-License-Identifier: MIT */
/** @file Config.h @brief Shared bounded stress-example settings. */
#pragma once
#include "demo_selection.h"
#include <KymoCore.h>
#ifndef KYMO_STRESS_CHANNEL_COUNT
/** @brief Number of unique wire IDs, 1..256; override with a build definition. */
#define KYMO_STRESS_CHANNEL_COUNT 256
#endif
#ifndef KYMO_STRESS_PERIOD_MS
/** @brief Per-channel interval; zero sends as fast as the transport permits. */
#define KYMO_STRESS_PERIOD_MS 0
#endif
#ifndef KYMO_STRESS_BAUD
/** @brief USB UART baud rate; must match the Studio connection. */
#define KYMO_STRESS_BAUD 2000000
#endif
#ifndef KYMO_STRESS_TCP_ENABLE
/** @brief Select TCP (1) or USB UART (0). */
#define KYMO_STRESS_TCP_ENABLE 0
#endif
#ifndef KYMO_STRESS_UDP_ENABLE
/** @brief Send UDP datagrams (1) instead of USB UART or TCP. */
#define KYMO_STRESS_UDP_ENABLE 0
#endif
#ifndef KYMO_STRESS_UDP_PORT
/** @brief UDP port KymoStudio listens on. */
#define KYMO_STRESS_UDP_PORT 2324
#endif
#ifndef KYMO_STRESS_XYZ_ENABLE
/** @brief Include X and Z in addition to Y and the millisecond timestamp. */
#define KYMO_STRESS_XYZ_ENABLE 0
#endif

#if (KYMO_STRESS_CHANNEL_COUNT < 1) || (KYMO_STRESS_CHANNEL_COUNT > 256)
#error "KYMO_STRESS_CHANNEL_COUNT must be 1..256"
#endif
#if KYMO_STRESS_CHANNEL_COUNT > KYMO_MAX_CHANNELS
#error "Set KYMO_MAX_CHANNELS to at least KYMO_STRESS_CHANNEL_COUNT"
#endif
#if (KYMO_STRESS_PERIOD_MS < 0) || (KYMO_STRESS_PERIOD_MS > 2147483647)
#error "KYMO_STRESS_PERIOD_MS must be 0..INT32_MAX"
#endif
#if (KYMO_STRESS_BAUD < 1)
#error "KYMO_STRESS_BAUD must be positive"
#endif
#if ((KYMO_STRESS_TCP_ENABLE != 0) && (KYMO_STRESS_TCP_ENABLE != 1)) || \
    ((KYMO_STRESS_XYZ_ENABLE != 0) && (KYMO_STRESS_XYZ_ENABLE != 1)) || \
    ((KYMO_STRESS_UDP_ENABLE != 0) && (KYMO_STRESS_UDP_ENABLE != 1))
#error "Stress feature switches must be 0 or 1"
#endif
#if KYMO_STRESS_TCP_ENABLE && KYMO_STRESS_UDP_ENABLE
#error "Select either the TCP or the UDP stress transport"
#endif
/** @brief WiFi is needed by both network transports. */
#define KYMO_STRESS_WIFI (KYMO_STRESS_TCP_ENABLE || KYMO_STRESS_UDP_ENABLE)
#if !KYMO_ENABLE_RUNTIME || !KYMO_ENABLE_TIMESTAMP
#error "Stress demo requires KYMO_ENABLE_RUNTIME=1 and KYMO_ENABLE_TIMESTAMP=1"
#endif
#if KYMO_STRESS_XYZ_ENABLE && (!KYMO_ENABLE_X || !KYMO_ENABLE_Z)
#error "XYZ stress demo requires KYMO_ENABLE_X=1 and KYMO_ENABLE_Z=1"
#endif
#if defined(KYMO_DEMO_NETWORK_ENABLE) && KYMO_DEMO_NETWORK_ENABLE
#error "Select either the network demo or the stress demo"
#endif

