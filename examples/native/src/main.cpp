/* SPDX-License-Identifier: MIT */
/** @file main.cpp @brief Run the portable Endpoint application, then drain it. */
#include <probe/kymo_init.h>
#include "kymo_plot.h"
#include "kymo_port.h"
static ProbeCore core;
static KymoEndpointPlot plot;
/*******************************************************************************
 * main
 ******************************************************************************/
int main(void)
{
    const KymoEndpointConfig port = kymoPortConfig();
    const KymoEndpointPlotConfig config = kymoPlotConfig();
    KymoStatus status = probeCoreInit(&core, &port);
    int result = 1;
    if (status == KYMO_OK) { status = KymoEndpointPlot_Init(&core.endpoint, &plot, &config); }
    if (status == KYMO_OK) {
        for (uint32_t step = 0u; step < 200u; ++step) {
            nativeTickMs = step;
            status = KymoEndpoint_Main(&core.endpoint);
        }
        status = KymoEndpoint_RequestStop(&core.endpoint);
        for (uint32_t drain = 0u; drain < 100u && status != KYMO_IDLE; ++drain) {
            status = KymoEndpoint_Main(&core.endpoint);
        }
        if ((status == KYMO_IDLE) && (nativeFrameCount == 10u)) { result = 0; }
    }
    return result;
}
