/* SPDX-License-Identifier: MIT */
/** @file kymo_port.cpp @brief Native clock and frame validation. */
#include "kymo_port.h"
#include <telemetry/kymo_protocol.h>
uint32_t nativeTickMs;
uint32_t nativeFrameCount;
/*******************************************************************************
 * readClock
 ******************************************************************************/
static uint32_t readClock(void *user)
{
    (void)user;
    return nativeTickMs;
}
/*******************************************************************************
 * writeFrame
 ******************************************************************************/
static size_t writeFrame(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    KymoDataPoint point = {};
    (void)user;
    if ((KymoProtocol_Decode(bytes, length, &point) == KYMO_OK) && (point.id == 7u) &&
        (point.value > 1.24f) && (point.value < 1.26f)) {
        ++nativeFrameCount;
        accepted = length;
    }
    return accepted;
}
/*******************************************************************************
 * kymoPortConfig
 ******************************************************************************/
KymoEndpointConfig kymoPortConfig(void)
{
    KymoEndpointConfig config = {};
    config.clock_ms = readClock;
    config.tx.write = writeFrame;
    return config;
}
