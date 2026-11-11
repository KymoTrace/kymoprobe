/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief ESP32 UART runtime trace example. */
#include "demo_selection.h"
#include <Arduino.h>
#include <KymoCore.h>

#if ((KYMO_DEMO_MODE == KYMO_DEMO_TRACE) || (KYMO_DEMO_MODE == KYMO_DEMO_TRACE_FLOW)) && KYMO_EFFECTIVE_TRACE
#include <esp_random.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/task.h>

#include "features/trace/kymo_trace.h"
#include "features/trace/kymo_port.h"
#include <probe/kymo_init.h>
static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;
static const uint32_t ENDPOINT_TURNS_PER_LOOP = 8u;


/** Pause per loop in ms. A loop records five events, sent as two batch frames
 *  (~340 bytes); 100 loops/s (~34 KB/s) fit UART 460800 (~46 KB/s). Without
 *  it the loop outruns the link and the recorder drops most records. */
#ifndef TRACE_DEMO_LOOP_DELAY_MS
#define TRACE_DEMO_LOOP_DELAY_MS 10u
#endif

/** Serial TX buffer in bytes. The default (UART FIFO, ~128 bytes) holds less
 *  than one batch frame, so the trace service could send only one frame per
 *  loop and the ring overflowed. */
#ifndef TRACE_DEMO_TX_BUFFER_BYTES
#define TRACE_DEMO_TX_BUFFER_BYTES 1024u
#endif



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
#if KYMO_EFFECTIVE_TRACE_FLOW
    const uint32_t correlation = iteration + 1u;
    KYMO_TRACE_FLOW_SEND(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                        KYMO_SYMBOL_FLOW_PRODUCER, correlation);
#endif
    for (volatile uint32_t step = 0u; step < 300u; ++step) {
        (void)step;
    }
#if KYMO_EFFECTIVE_TRACE_FLOW
    KYMO_TRACE_FLOW_RECEIVE(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                           KYMO_SYMBOL_FLOW_CONSUMER, correlation);
#endif
}


/*******************************************************************************
 * demoTraceInit
 ******************************************************************************/
void demoTraceInit(void)
{
    (void)Serial.setTxBufferSize(TRACE_DEMO_TX_BUFFER_BYTES); /* before begin */
    Serial.begin(460800u);
    kymoTracePortInit();
    const KymoEndpointConfig port = kymoPortConfig();
    const KymoEndpointTraceConfig traceConfig = kymoTraceConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = kymoTraceInit(&core.endpoint, &traceConfig); }

}

/*******************************************************************************
 * demoTraceMain
 ******************************************************************************/
void demoTraceMain(void)
{
    delay(TRACE_DEMO_LOOP_DELAY_MS); /* outside the scope: not loop run time */
    KYMO_TRACE_SCOPE(kymoTraceRecorder(), KYMO_CONTEXT_MAIN,
                     KYMO_SYMBOL_MAIN_LOOP);
    traceInstrumentedWork(loopCounter++);
    for (uint32_t turn = 0u; turn < ENDPOINT_TURNS_PER_LOOP; ++turn) {
        status = KymoEndpoint_Main(&core.endpoint);
    }
    yield();
}

#endif /* selected trace application */
