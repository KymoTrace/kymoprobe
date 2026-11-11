/* SPDX-License-Identifier: MIT */
/** @file kymo_watch.cpp @brief Allowed memory and firmware identity for live Watch. */
#include "kymo_watch.h"
#include <esp_ota_ops.h>
#include <string.h>
extern "C" uint8_t _data_start;
extern "C" uint8_t _bss_end;

static KymoWatchRange ranges[1];
static uint8_t buildId[16];
/*******************************************************************************
 * readBuildId
 ******************************************************************************/
static size_t readBuildId(void)
{
    const esp_app_desc_t *description = esp_ota_get_app_description();
    size_t length = 0u;
    memcpy(buildId, description->app_elf_sha256, sizeof(buildId));
    for (size_t index = 0u; index < sizeof(buildId); ++index) {
        if (buildId[index] != 0u) {
            length = sizeof(buildId);
        }
    }
    return length;
}
/*******************************************************************************
 * kymoWatchConfig
 ******************************************************************************/
KymoEndpointWatchConfig kymoWatchConfig(void)
{
    KymoEndpointWatchConfig config = {};
    ranges[0].start = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&_data_start));
    ranges[0].length = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&_bss_end) - reinterpret_cast<uintptr_t>(&_data_start));
    config.watch.ranges = ranges;
    config.watch.range_count = KYMO_COUNT_OF(ranges);
    config.watch.build_id = buildId;
    config.watch.build_id_length = readBuildId();
    config.first_channel = 0u;
    config.last_channel = UINT8_MAX;
    return config;
}

static KymoEndpointWatch feature;
/*******************************************************************************
 * kymoWatchInit
 ******************************************************************************/
KymoStatus kymoWatchInit(KymoEndpoint *endpoint, const KymoEndpointWatchConfig *config)
{
    KymoStatus status = KymoEndpointWatch_Init(endpoint, &feature, config);
    return status;
}
