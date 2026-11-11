/* SPDX-License-Identifier: MIT */
/** @file main.cpp
 * @brief Select the required features, then run one common Main function.
 * @details Arduino superloop, no parallel telemetry calls. Fatal errors stop
 * processing; inspect sStatus with a debugger or add application error handling.
 */
#if defined(ARDUINO)
#include <Arduino.h> /* Framework setup/loop declarations, including AVR linkage. */
#endif
#include "kymo_init.h"
#include "features/kymo_plot.h"
#include "features/kymo_param.h"
#include "features/kymo_watch.h"
#include "features/kymo_memory.h"
#include "features/kymo_trace.h"
#include "kymo_port.h"

static KymoStatus sStatus = KYMO_BAD_CONFIG;

/** @brief Initialize the port, common base and selected features.
 * @par Usage Arduino calls this once before loop. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    const KymoEndpointConfig coreConfig = kymoCoreConfig();
    const KymoEndpointPlotConfig plotConfig = kymoPlotConfig();
#if KYMO_EFFECTIVE_PARAMETERS
    const KymoEndpointParamConfig paramConfig = kymoParamConfig();
#endif
#if KYMO_EFFECTIVE_WATCH
    const KymoEndpointWatchConfig watchConfig = kymoWatchConfig();
#endif
#if KYMO_EFFECTIVE_MEMORY
    const KymoEndpointMemoryConfig memoryConfig = kymoMemoryConfig();
#endif
#if KYMO_EFFECTIVE_TRACE
    const KymoEndpointTraceConfig traceConfig = kymoTraceConfig();
#endif
    kymoPortInit();
    sStatus = KymoApp_Init(&coreConfig);
    if (sStatus == KYMO_OK) {
        sStatus = KymoApp_PlotInit(&plotConfig);
    }
#if KYMO_EFFECTIVE_PARAMETERS
    if (sStatus == KYMO_OK) {
        sStatus = KymoApp_ParamInit(&paramConfig);
    }
#endif
#if KYMO_EFFECTIVE_WATCH
    if (sStatus == KYMO_OK) {
        sStatus = KymoApp_WatchInit(&watchConfig);
    }
#endif
#if KYMO_EFFECTIVE_MEMORY
    if (sStatus == KYMO_OK) {
        sStatus = KymoApp_MemoryInit(&memoryConfig);
    }
#endif
#if KYMO_EFFECTIVE_TRACE
    if (sStatus == KYMO_OK) {
        sStatus = KymoApp_TraceInit(&traceConfig);
    }
#endif
}
/** @brief Run one processing cycle while the previous result permits progress.
 * @par Usage Arduino calls this repeatedly; no active wait or delay. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    if ((sStatus == KYMO_OK) || (sStatus == KYMO_IDLE) ||
        (sStatus == KYMO_BUSY) || (sStatus == KYMO_BAD_SAMPLE)) {
#if KYMO_EFFECTIVE_TRACE
        /* Application instrumentation; the Endpoint handles all trace output. */
        KymoApp_TraceHeartbeat();
#endif
        sStatus = KymoApp_Main();
    }
}
