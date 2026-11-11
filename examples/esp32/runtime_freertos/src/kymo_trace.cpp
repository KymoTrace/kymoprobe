/** @file kymo_trace.cpp @brief Feature-owned recorder and metadata configuration. */
#include <KymoCore.h>
#if KYMO_EFFECTIVE_TRACE
#include "kymo_trace.h"
#include "kymo_port.h"
#include <kymo_trace_metadata.h>
static KymoEndpointTrace trace;
static KymoTraceRecorder *recorder;
/*******************************************************************************
 * kymoTraceConfig
 ******************************************************************************/
KymoEndpointTraceConfig kymoTraceConfig(void)
{
    const uint32_t RECORDER_TICKS_PER_SECOND = 1000000u;
    const uint32_t METADATA_PERIOD_SECONDS = 5u;
    const uint32_t HEALTH_PERIOD_TICKS = 10000u;
    KymoEndpointTraceConfig config = {};
    config.recorder = kymoRecorderConfig();
    config.metadata = kymo_trace_session_metadata;
    config.metadata_length = kymo_trace_session_metadata_length;
    config.dictionary = kymo_trace_dictionary;
    config.dictionary_length = kymo_trace_dictionary_length;
    config.generation = 1u;
    config.health_period_ticks = HEALTH_PERIOD_TICKS;
    config.metadata_period_ticks = METADATA_PERIOD_SECONDS * RECORDER_TICKS_PER_SECOND;
    return config;
}
/*******************************************************************************
 * kymoTraceInit
 ******************************************************************************/
KymoStatus kymoTraceInit(KymoEndpoint *endpoint, const KymoEndpointTraceConfig *config)
{
    KymoStatus status = KymoEndpointTrace_Init(endpoint, &trace, config);
    if (status == KYMO_OK) { recorder = KymoEndpointTrace_Recorder(&trace); }
    return status;
}
/*******************************************************************************
 * kymoTraceRecorder
 ******************************************************************************/
KymoTraceRecorder *kymoTraceRecorder(void)
{
    return recorder;
}
static uint64_t healthTicks;
/*******************************************************************************
 * kymoTraceDiagnostics
 ******************************************************************************/
void kymoTraceDiagnostics(bool startupOk)
{
    const uint64_t now = traceClockTicks(nullptr);
    const uint32_t DIAGNOSTIC_PERIOD_TICKS = 1000000u;
    if ((recorder != nullptr) && ((now - healthTicks) >= DIAGNOSTIC_PERIOD_TICKS)) {
        const KymoTraceConfig &port = trace.config.recorder;
        port.critical_enter(port.user);
        const uint32_t drops = recorder->drops[0u] + recorder->drops[1u];
        const uint32_t errors = recorder->errors[0u] + recorder->errors[1u];
        port.critical_exit(port.user);
        KYMO_TRACE_MARK(recorder, KYMO_CONTEXT_MAIN, KYMO_SYMBOL_RECORDER_DROPS, drops);
        KYMO_TRACE_MARK(recorder, KYMO_CONTEXT_MAIN, KYMO_SYMBOL_RECORDER_ERRORS, errors);
        KYMO_TRACE_MARK(recorder, KYMO_CONTEXT_MAIN, KYMO_SYMBOL_STARTUP_OK, startupOk ? 1u : 0u);
        healthTicks = now;
    }
}
#endif
