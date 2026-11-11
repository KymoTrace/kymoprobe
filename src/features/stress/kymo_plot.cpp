/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Stress channel configuration and deterministic ramp bindings. */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_STRESS_ENABLE && KYMO_EFFECTIVE_RUNTIME
#include "Config.h"
#include "kymo_plot.h"
#include <string.h>
static KymoChannel s_channels[KYMO_STRESS_CHANNEL_COUNT];
static uint32_t s_sampleCounts[KYMO_STRESS_CHANNEL_COUNT];
/*******************************************************************************
 * readStressSample
 ******************************************************************************/
static uint8_t readStressSample(void *user, uint8_t id, KymoSample *out)
{
    uint8_t result = 0u;
    uint32_t phase = 0u;
    bool isValidId = true;
    (void)user;
#if KYMO_STRESS_CHANNEL_COUNT < 256
    isValidId = id < KYMO_STRESS_CHANNEL_COUNT;
#endif
    if ((out != nullptr) && isValidId) {
        phase = (s_sampleCounts[id] + static_cast<uint32_t>(id) * 17u) % 2048u;
        out->value = static_cast<float>(phase) / 1023.5f - 1.0f;
#if KYMO_STRESS_XYZ_ENABLE
        out->x = static_cast<float>((phase + 512u) % 2048u) / 1023.5f - 1.0f;
        out->z = static_cast<float>((phase + 1024u) % 2048u) / 1023.5f - 1.0f;
#endif
        ++s_sampleCounts[id];
        result = 1u;
    }
    return result;
}
/*******************************************************************************
 * stressPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig stressPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    (void)memset(s_sampleCounts, 0, sizeof(s_sampleCounts));
    for (uint16_t id = 0u; id < KYMO_STRESS_CHANNEL_COUNT; ++id) {
        s_channels[id].id = static_cast<uint8_t>(id);
        s_channels[id].period_ms = KYMO_STRESS_PERIOD_MS;
        s_channels[id].flags = KYMO_FLAG_TIMESTAMP;
#if KYMO_STRESS_XYZ_ENABLE
        s_channels[id].flags = KYMO_FLAG_TIMESTAMP | KYMO_FLAG_X | KYMO_FLAG_Z;
#endif
    }
    config.channels = s_channels;
    config.channel_count = KYMO_COUNT_OF(s_channels);
    config.sample = readStressSample;
    return config;
}
#endif
