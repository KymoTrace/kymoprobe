/* SPDX-License-Identifier: MIT */
/** @file wifi_mqtt_example.ino @brief Endpoint startup and cooperative processing. */
#include <Arduino.h>
#include <probe/kymo_init.h>
#include "src/kymo_port.h"
#include "src/kymo_plot.h"
static ProbeCore core;
static KymoEndpointPlot plot;
static KymoStatus status = KYMO_BAD_CONFIG;
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    const KymoEndpointConfig port = kymoMqttPortConfig();
    const KymoEndpointPlotConfig config = kymoPlotConfig();
    kymoMqttPortInit();
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
