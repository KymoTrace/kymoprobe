/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h @brief STM32 waveform feature; persistent channels and callback. */
#ifndef PROBE_STM32_PLOT_H
#define PROBE_STM32_PLOT_H
#include <endpoint/features/kymo_plot.h>
/** @brief Configure the waveform feature. @return Persistent plot bindings. */
KymoEndpointPlotConfig kymoPlotConfig(void);
#endif
