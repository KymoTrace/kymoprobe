/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Waveform channels and bindings, separate from the port. */
#include "probe/kymo_plot.h"
#include "probe/kymo_port.h"
#include <common/embedded_bits.h>
#include <math.h>
#if KYMO_EFFECTIVE_ENDPOINT && KYMO_EFFECTIVE_RUNTIME && defined(ARDUINO)
static KymoEndpointPlot feature;
static uint8_t example_sample(void *user, uint8_t id, KymoSample *out) {
    (void)user;
    const float phase = (probeSerialClock(nullptr) % 2000u) / 2000.0f;
#if KYMO_ENABLE_X
    out->x = phase;
#endif
    out->value = id == 0 ? sinf(phase * 6.283185307f) : 2.0f * phase - 1.0f;
#if KYMO_ENABLE_Z
    out->z = cosf(phase * 6.283185307f);
#endif
    return 1;
}
static const KymoChannel example_channels[] = {
    {20, 0, KYMO_FLAG_TIMESTAMP},
#ifdef KYMO_EXAMPLE_MULTIDIMENSIONAL
    {50, 1, EMB_U8_OR(KYMO_FLAG_X, KYMO_FLAG_TIMESTAMP)},
    {100, 2, KYMO_ALLOWED_FLAGS},
#else
    {20, 1, KYMO_FLAG_TIMESTAMP},
#endif
};
/*******************************************************************************
 * probeWaveformConfig
 ******************************************************************************/
KymoEndpointPlotConfig probeWaveformConfig(void)
{
    KymoEndpointPlotConfig config = {};
    config.channels = example_channels;
    config.channel_count = KYMO_COUNT_OF(example_channels);
    config.sample = example_sample;
    return config;
}
/*******************************************************************************
 * probeWaveformInit
 ******************************************************************************/
KymoStatus probeWaveformInit(KymoEndpoint *endpoint, const KymoEndpointPlotConfig *config)
{
    KymoStatus status = KymoEndpointPlot_Init(endpoint, &feature, config);
    return status;
}
#endif
