/* SPDX-License-Identifier: MIT */
/** @file kymo_watch.cpp
 * @brief Watch-specific bindings and complete initialization.
 */
#include "features/kymo_watch.h"
#if KYMO_EFFECTIVE_WATCH
#include "../detail/kymo_internal.h"
#include "app_values.h"
#include <string.h>
static KymoEndpointWatch sFeature;
static const uint32_t HOST_WATCH_ADDRESS = 0x20000000u;
/* Hardware uses the actual ELF address. A 64-bit host uses a mapped demo address;
   the bounded reader below never dereferences a received address. */
static const uint32_t WATCH_GAIN_ADDRESS = sizeof(uintptr_t) <= sizeof(uint32_t) ?
    static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&appGain)) : HOST_WATCH_ADDRESS;
static const uint8_t WATCH_CHANNEL = 150u;
static const KymoWatchRange RANGES[] = { { WATCH_GAIN_ADDRESS, sizeof(appGain) } };
/*******************************************************************************
 * readWatch
 ******************************************************************************/
static uint8_t readWatch(void *user, uint32_t address, uint8_t size, uint8_t *output)
{
    uint8_t result = 0u;
    (void)user;
    if ((output != nullptr) && (address == WATCH_GAIN_ADDRESS) && (size == sizeof(appGain))) {
        memcpy(output, &appGain, sizeof(appGain));
        result = 1u;
    }
    return result;
}
/*******************************************************************************
 * kymoWatchConfig
 ******************************************************************************/
KymoEndpointWatchConfig kymoWatchConfig(void)
{
    KymoEndpointWatchConfig config = {};
    config.watch.ranges = RANGES;
    config.watch.range_count = KYMO_COUNT_OF(RANGES);
    config.watch.read = readWatch;
    config.first_channel = WATCH_CHANNEL;
    config.last_channel = WATCH_CHANNEL;
    return config;
}
/*******************************************************************************
 * KymoApp_WatchInit
 ******************************************************************************/
KymoStatus KymoApp_WatchInit(const KymoEndpointWatchConfig *config)
{
    KymoStatus status = KymoEndpointWatch_Init(kymoEndpoint(), &sFeature, config);
    return status;
}

#endif /* enabled feature */
