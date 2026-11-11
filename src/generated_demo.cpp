/* SPDX-License-Identifier: MIT */
/** @file generated_demo.cpp @brief Configurator-driven UART demonstration. */
#include "demo_selection.h"
#include <KymoCore.h>
#if (KYMO_DEMO_MODE == KYMO_DEMO_GENERATED) && KYMO_EFFECTIVE_RUNTIME
#include <Arduino.h>
#include <kymo_generated_config.h>
#include <probe/kymo_init.h>
#include "features/generated/kymo_plot.h"
static ProbeCore generatedContext;
static KymoEndpointPlot generatedPlot;
static KymoStatus generatedStatus = KYMO_BAD_CONFIG;
/*******************************************************************************
 * writeGenerated
 ******************************************************************************/
static size_t writeGenerated(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    if ((length <= UINT8_MAX) && (kymo_generated_config.write != nullptr)) {
        accepted = kymo_generated_config.write(user, bytes, static_cast<uint8_t>(length));
    }
    return accepted;
}


/** @brief Start hardware before initializing the generated configuration. */
/*******************************************************************************
 * demoGeneratedInit
 ******************************************************************************/
void demoGeneratedInit(void)
{
    Serial.begin(115200u);
    KymoEndpointConfig port = {};
    const KymoEndpointPlotConfig plot = generatedPlotConfig();
    port.clock_ms = kymo_generated_config.clock_ms;
    port.clock_user = kymo_generated_config.clock_user;
    port.tx.write = writeGenerated;
    port.tx.busy = kymo_generated_config.busy;
    port.tx.transport_user = kymo_generated_config.transport_user;
    port.service = kymo_generated_config.service;
    generatedStatus = probeCoreInit(&generatedContext, &port);
    if (generatedStatus == KYMO_OK) {
        generatedStatus = KymoEndpointPlot_Init(&generatedContext.endpoint, &generatedPlot, &plot);
    }
}

/** @brief Advance the runtime selected by the immutable profile. */
/*******************************************************************************
 * demoGeneratedMain
 ******************************************************************************/
void demoGeneratedMain(void)
{
    generatedStatus = KymoEndpoint_Main(&generatedContext.endpoint);
}
#endif
