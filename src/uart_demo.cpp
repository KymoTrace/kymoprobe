/* SPDX-License-Identifier: MIT */
/** @file uart_demo.cpp @brief Two timestamped UART waveform channels. */
#include "demo_selection.h"
#include <KymoCore.h>
#if (KYMO_DEMO_MODE == KYMO_DEMO_UART) && KYMO_EFFECTIVE_RUNTIME
#include <Arduino.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include <probe/kymo_plot.h>
static ProbeCore uartContext;
static KymoStatus uartStatus = KYMO_BAD_CONFIG;

/** @brief Start UART and its fixed runtime configuration. */
/*******************************************************************************
 * demoUartInit
 ******************************************************************************/
void demoUartInit(void)
{
    Serial.begin(115200u);
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointPlotConfig plot = probeWaveformConfig();
    uartStatus = probeCoreInit(&uartContext, &port);
    if (uartStatus == KYMO_OK) { uartStatus = probeWaveformInit(&uartContext.endpoint, &plot); }
}

/** @brief Advance acquisition and transmission cooperatively. */
/*******************************************************************************
 * demoUartMain
 ******************************************************************************/
void demoUartMain(void)
{
    uartStatus = KymoEndpoint_Main(&uartContext.endpoint);
}
#endif
