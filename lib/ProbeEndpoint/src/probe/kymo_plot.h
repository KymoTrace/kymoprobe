/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief Shared waveform Plot feature for simple UART/USB demos.
 * @details No other feature is configured here. Build flags select Y or XYZ.
 * @par Usage Init base, obtain config and initialize Plot once. */
#pragma once
#include <endpoint/features/kymo_plot.h>
#if KYMO_EFFECTIVE_ENDPOINT && KYMO_EFFECTIVE_RUNTIME
/** @brief Prepare the two- or three-channel waveform configuration.
 * @return Persistent channel table and sampler. */
KymoEndpointPlotConfig probeWaveformConfig(void);
/** @brief Initialize the shared waveform feature.
 * @param[in,out] endpoint Initialized base. @param[in] config Plot settings.
 * @return Feature Init status. */
KymoStatus probeWaveformInit(KymoEndpoint *endpoint, const KymoEndpointPlotConfig *config);
#endif
