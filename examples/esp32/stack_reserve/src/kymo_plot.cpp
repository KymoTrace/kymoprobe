/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Stack watermark and parser-depth channels. */
#include "kymo_plot.h"
#include <Arduino.h>
extern TaskHandle_t loopHandle, sensorHandle, parserHandle;
extern volatile uint32_t parserDepth;
static const uint32_t REPORT_PERIOD_MS = 500u;
static const KymoChannel channels[] = {
    {REPORT_PERIOD_MS, 0u, KYMO_FLAG_TIMESTAMP}, {REPORT_PERIOD_MS, 1u, KYMO_FLAG_TIMESTAMP},
    {REPORT_PERIOD_MS, 2u, KYMO_FLAG_TIMESTAMP}, {REPORT_PERIOD_MS, 3u, KYMO_FLAG_TIMESTAMP}
};
/*******************************************************************************
 * readStack
 ******************************************************************************/
static uint8_t readStack(void *user, uint8_t id, KymoSample *sample)
{
    uint8_t valid = 0u;
    TaskHandle_t task = nullptr;
    (void)user;
    if (sample != nullptr) {
        switch (id) {
        case 0u: task = loopHandle; break;
        case 1u: task = sensorHandle; break;
        case 2u: task = parserHandle; break;
        case 3u: sample->value = static_cast<float>(parserDepth); valid = 1u; break;
        default: break;
        }
        if (task != nullptr) { sample->value = static_cast<float>(uxTaskGetStackHighWaterMark(task)); valid = 1u; }
    }
    return valid;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = readStack;
    return config;
}
