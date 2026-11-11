/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp
 * @brief One Y channel; replace readMeasurement with your own measurement.
 */
#include "features/kymo_plot.h"
#include "app_values.h"
#include "kymo_port.h"
#include "../detail/kymo_internal.h"

static const uint8_t MEASUREMENT_CHANNEL = 0u;
static const uint32_t SAMPLE_PERIOD_MS = 50u;
static const uint32_t SIGNAL_PERIOD_MS = 1000u;
static const KymoChannel CHANNELS[] = {
    { SAMPLE_PERIOD_MS, MEASUREMENT_CHANNEL, 0u }
};
static KymoEndpointPlot sPlot;

/** @brief Read a deterministic ramp from 0 up to, but excluding, appGain.
 * @details Demonstration data, not a sensor. Float conversion is required by the
 * DATA protocol; on AVR this uses software floating point, only at sample time.
 * @param[in] user Unused; NULL allowed.
 * @param[in] channel Requested configured channel.
 * @param[out] sample Sample destination; NULL is rejected.
 * @return 1 for a valid sample, 0 to skip. */
/*******************************************************************************
 * readMeasurement
 ******************************************************************************/
static uint8_t readMeasurement(void *user, uint8_t channel, KymoSample *sample)
{
    uint8_t valid = 0u;
    uint32_t phaseMs = kymoPortClockMs(nullptr) % SIGNAL_PERIOD_MS;
    (void)user;
    if ((sample != nullptr) && (channel == MEASUREMENT_CHANNEL)) {
        sample->value = appGain * static_cast<float>(phaseMs) /
            static_cast<float>(SIGNAL_PERIOD_MS);
        valid = 1u;
    }
    return valid;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    config.channels = CHANNELS;
    config.channel_count = KYMO_COUNT_OF(CHANNELS);
    config.sample = readMeasurement;
    return config;
}
/*******************************************************************************
 * KymoApp_PlotInit
 ******************************************************************************/
KymoStatus KymoApp_PlotInit(const KymoEndpointPlotConfig *config)
{
    KymoStatus status = KymoEndpointPlot_Init(kymoEndpoint(), &sPlot, config);
    return status;
}
