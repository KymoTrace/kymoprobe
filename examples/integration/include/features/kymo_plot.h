/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.h
 * @brief Plot-only configuration and initialization.
 * @details Channels and sample callbacks live in the corresponding CPP file.
 * @par Usage Configure and initialize Plot after the common base.
 */
#ifndef EXAMPLE_KYMO_PLOT_H
#define EXAMPLE_KYMO_PLOT_H
#include <endpoint/features/kymo_plot.h>
/** @brief Prepare one demonstration channel and its sample callback.
 * @return Plot configuration referencing persistent resources.
 * @par Usage Pass to KymoApp_PlotInit or supply your own configuration. */
KymoEndpointPlotConfig kymoPlotConfig(void);
/** @brief Initialize and register Plot completely.
 * @param[in] config Plot settings; tables and callback context must persist.
 * @return Library feature initialization status. @par Usage Once after base Init. */
KymoStatus KymoApp_PlotInit(const KymoEndpointPlotConfig *config);
#endif
