/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief ESP32 UART runtime trace example. */
#include <Arduino.h>
#include <esp_random.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/task.h>
#include <KymoCore.h>

#include "kymo_trace.h"
#include "kymo_port.h"
#include <probe/kymo_init.h>
static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;
static const uint32_t ENDPOINT_TURNS_PER_LOOP = 8u;




uint32_t loopCounter = 0u;










/*******************************************************************************
 * traceInstrumentedWork
 ******************************************************************************/
void traceInstrumentedWork(uint32_t iteration)
{
    KYMO_TRACE_SCOPE(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                     KYMO_SYMBOL_SIMULATED_WORK);
    KYMO_TRACE_MARK(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                    KYMO_SYMBOL_ITERATION, iteration);
    for (volatile uint32_t step = 0u; step < 300u; ++step) {
        (void)step;
    }
}


/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.begin(460800u);
    kymoTracePortInit();
    const KymoEndpointConfig port = kymoPortConfig();
    const KymoEndpointTraceConfig traceConfig = kymoTraceConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = kymoTraceInit(&core.endpoint, &traceConfig); }

}

/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    const uint32_t LOOP_PERIOD_MS = 10u;
    delay(LOOP_PERIOD_MS);
    KYMO_TRACE_SCOPE(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                     KYMO_SYMBOL_MAIN_LOOP);
    traceInstrumentedWork(loopCounter++);
    for (uint32_t turn = 0u; turn < ENDPOINT_TURNS_PER_LOOP; ++turn) {
        status = KymoEndpoint_Main(&core.endpoint);
    }
    yield();
}
