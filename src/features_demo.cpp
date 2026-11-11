/* SPDX-License-Identifier: MIT */
/** @file features_demo.cpp @brief Endpoint-driven Y/XY/XYZ/CRC demonstration. */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_DEMO_MODE == KYMO_DEMO_FEATURES && KYMO_ENABLED
#include <Arduino.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include "features/feature/kymo_plot.h"
static ProbeCore featureCore;
static KymoEndpointPlot featurePlot;
static KymoStatus featureStatus = KYMO_BAD_CONFIG;
/*******************************************************************************
 * demoFeaturesInit
 ******************************************************************************/
void demoFeaturesInit(void)
{
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointPlotConfig plot = kymoPlotConfig();
    Serial.begin(115200u);
    featureStatus = probeCoreInit(&featureCore, &port);
    if (featureStatus == KYMO_OK) { featureStatus = KymoEndpointPlot_Init(&featureCore.endpoint, &featurePlot, &plot); }
}
/*******************************************************************************
 * demoFeaturesMain
 ******************************************************************************/
void demoFeaturesMain(void)
{
    featureStatus = KymoEndpoint_Main(&featureCore.endpoint);
}
#endif
