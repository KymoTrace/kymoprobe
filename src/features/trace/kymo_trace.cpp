/** @file kymo_trace.cpp @brief Feature-owned recorder and metadata configuration. */
#include "demo_selection.h"
#include <KymoCore.h>
#if ((KYMO_DEMO_MODE == KYMO_DEMO_TRACE) || (KYMO_DEMO_MODE == KYMO_DEMO_TRACE_FLOW)) && KYMO_EFFECTIVE_TRACE
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
#endif
