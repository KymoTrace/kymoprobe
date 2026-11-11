/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Controller measurement channels; Endpoint owns sampling and TX. */
#include "kymo_plot.h"
#include "app_values.h"
static const uint32_t REPORT_PERIOD_MS = 20u;
static const KymoChannel channels[] = {
    {REPORT_PERIOD_MS, 0u, KYMO_FLAG_TIMESTAMP}, {REPORT_PERIOD_MS, 1u, KYMO_FLAG_TIMESTAMP},
    {REPORT_PERIOD_MS, 2u, KYMO_FLAG_TIMESTAMP}, {REPORT_PERIOD_MS, 3u, KYMO_FLAG_TIMESTAMP}
};
/*******************************************************************************
 * readPlot
 ******************************************************************************/
static uint8_t readPlot(void *user, uint8_t id, KymoSample *sample)
{
    uint8_t valid = 0u;
    (void)user;
    if (sample != nullptr) {
        valid = 1u;
        switch (id) {
        case 0u: sample->value = temperature; break;
        case 1u: sample->value = heater; break;
        case 2u: sample->value = setpoint; break;
        case 3u: sample->value = static_cast<float>(backChannelMaxUs); backChannelMaxUs = 0u; break;
        default: valid = 0u; break;
        }
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
    config.sample = readPlot;
    return config;
}

static KymoEndpointPlot feature;
/*******************************************************************************
 * kymoPlotInit
 ******************************************************************************/
KymoStatus kymoPlotInit(KymoEndpoint *endpoint, const KymoEndpointPlotConfig *config)
{
    KymoStatus status = KymoEndpointPlot_Init(endpoint, &feature, config);
    return status;
}
