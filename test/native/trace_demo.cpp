/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file trace_demo.cpp @brief Host-run KymoTrace end-to-end source. */
#include <chrono>
#include <cstdint>
#include <cstdio>
#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif
#include <KymoCore.h>
#include <endpoint/features/kymo_trace.h>
#include <assert.h>
#include <kymo_trace_ids.h>
#include <kymo_trace_metadata.h>

namespace {
constexpr uint16_t kClockDomain = 1u;
KymoEndpoint endpoint;
KymoEndpointTrace feature;
uint8_t txBuffer[KYMO_TRACE_FRAME_MAX];
uint64_t ticks = 1u;
uint64_t traceSessionId = 0u;


/*******************************************************************************
 * kymo_trace_session_id
 ******************************************************************************/
extern "C" uint64_t kymo_trace_session_id(void)
{
    return traceSessionId;
}

/*******************************************************************************
 * configureBinaryOutput
 ******************************************************************************/
void configureBinaryOutput(void)
{
#if defined(_WIN32)
    (void)_setmode(_fileno(stdout), _O_BINARY);
#endif
}

/*******************************************************************************
 * traceClockTicks
 ******************************************************************************/
uint64_t traceClockTicks(void *user)
{
    uint64_t result = ticks;
    (void)user;
    return result;
}

/*******************************************************************************
 * traceCoreId
 ******************************************************************************/
uint8_t traceCoreId(void *user)
{
    uint8_t result = 0u;
    (void)user;
    return result;
}

/*******************************************************************************
 * traceCurrentContext
 ******************************************************************************/
uint32_t traceCurrentContext(void *user)
{
    uint32_t result = KYMO_CONTEXT_MAIN;
    (void)user;
    return result;
}

/*******************************************************************************
 * traceWriteFrame
 ******************************************************************************/
size_t traceWriteFrame(void *user, const uint8_t *bytes, size_t length)
{
    size_t result = 0u;
    (void)user;
    if (bytes != nullptr) {
        const size_t WRITE_LIMIT = 17u;
        result = std::fwrite(bytes, 1u, length < WRITE_LIMIT ? length : WRITE_LIMIT, stdout);
    }
    return result;
}

/*******************************************************************************
 * traceIsBusy
 ******************************************************************************/
uint8_t traceIsBusy(void *user)
{
    uint8_t result = 0u;
    (void)user;
    return result;
}

/*******************************************************************************
 * traceInstrumentedWork
 ******************************************************************************/
void traceInstrumentedWork(uint32_t iteration)
{
    KYMO_TRACE_SCOPE(&feature.recorder, KYMO_CONTEXT_MAIN,
                     KYMO_SYMBOL_SIMULATED_WORK);
    KYMO_TRACE_MARK(&feature.recorder, KYMO_CONTEXT_MAIN,
                    KYMO_SYMBOL_ITERATION, iteration);
}

}

/*******************************************************************************
 * main
 ******************************************************************************/
int main(void)
{
    KymoEndpointConfig port = {};
    KymoEndpointTraceConfig config = {};
    configureBinaryOutput();
    traceSessionId = 1u;
    port.tx.buffer = txBuffer;
    port.tx.capacity = sizeof(txBuffer);
    port.tx.write = traceWriteFrame;
    port.tx.busy = traceIsBusy;
    config.recorder.clock_ticks = traceClockTicks;
    config.recorder.core_id = traceCoreId;
    config.recorder.current_context = traceCurrentContext;
    config.recorder.session_id = kymo_trace_session_id();
    config.recorder.clock_domain_id = kClockDomain;
    config.metadata = kymo_trace_session_metadata;
    config.metadata_length = kymo_trace_session_metadata_length;
    config.dictionary = kymo_trace_dictionary;
    config.dictionary_length = kymo_trace_dictionary_length;
    config.generation = 1u;
    config.metadata_period_ticks = 5000000u;
    assert(KymoEndpoint_Init(&endpoint, &port) == KYMO_OK);
    assert(KymoEndpointTrace_Init(&endpoint, &feature, &config) == KYMO_OK);
    for (uint32_t iteration = 0u; iteration < 128u; ++iteration) {
        {
            KYMO_TRACE_SCOPE(&feature.recorder, KYMO_CONTEXT_MAIN, KYMO_SYMBOL_MAIN_LOOP);
            traceInstrumentedWork(iteration);
        }
        ticks += 100000u;
        for (uint32_t turn = 0u; turn < 100u; ++turn) {
            (void)KymoEndpoint_Main(&endpoint);
        }
    }
    assert(KymoEndpoint_RequestStop(&endpoint) == KYMO_OK);
    for (uint32_t turn = 0u; turn < 4000u && endpoint.phase != KYMO_ENDPOINT_STOPPED; ++turn) {
        (void)KymoEndpoint_Main(&endpoint);
    }
    assert(endpoint.phase == KYMO_ENDPOINT_STOPPED);
    return 0;
}
