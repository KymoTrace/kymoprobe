/* SPDX-License-Identifier: MIT */
/** @file main.cpp @brief Four coherent Endpoint-scheduled numeric measurement signals. */
#include <Arduino.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include "Config.h"
#include "kymo_plot.h"
static ProbeCore core;
static KymoEndpointPlot plot;
static KymoStatus status = KYMO_BAD_CONFIG;
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointPlotConfig config = kymoPlotConfig();
    Serial.setTxBufferSize(MEASUREMENT_TX_BUFFER);
    Serial.begin(MEASUREMENT_BAUD);
    port.clock_ms = kymoPlotClock;
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = KymoEndpointPlot_Init(&core.endpoint, &plot, &config); }
}
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    status = KymoEndpoint_Main(&core.endpoint);
}
