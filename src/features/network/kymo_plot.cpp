/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Independent TCP and MQTT waveform bindings. */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_DEMO_NETWORK_ENABLE && KYMO_EFFECTIVE_RUNTIME
#include "kymo_plot.h"
#include <Arduino.h>
#include <math.h>
/*******************************************************************************
 * readSample
 ******************************************************************************/
static uint8_t readSample(void *user, uint8_t id, KymoSample *out)
{
    uint8_t result = 0u;
    const float phase = static_cast<float>(millis() % 2000u) / 2000.0f;
    (void)user;
    if (out != nullptr) {
        out->value = id == 2u ? sinf(phase * 6.283185307f) : 2.0f * phase - 1.0f;
        result = 1u;
    }
    return result;
}
/*******************************************************************************
 * networkPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig networkPlotConfig(bool tcp)
{
    static const KymoChannel tcpChannels[] = {{100u, 2u, KYMO_FLAG_TIMESTAMP}};
    static const KymoChannel mqttChannels[] = {{100u, 3u, KYMO_FLAG_TIMESTAMP}};
    KymoEndpointPlotConfig config = {};
    config.channels = tcp ? tcpChannels : mqttChannels;
    config.channel_count = 1u;
    config.sample = readSample;
    return config;
}
#endif
