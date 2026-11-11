/* SPDX-License-Identifier: MIT */
/** @file kymo_trace.cpp
 * @brief Trace-specific bindings and complete initialization.
 */
#include "features/kymo_trace.h"
#if KYMO_EFFECTIVE_TRACE
#include "../detail/kymo_internal.h"
#include "kymo_port.h"
#include "detail/kymo_trace_records.h"
static KymoEndpointTrace sFeature;
static const uint64_t TRACE_SESSION_ID = 1u;
static const uint16_t TRACE_CLOCK_DOMAIN_ID = 0u;
static const uint32_t TRACE_CONTEXT_ID = 1u;
static const uint16_t TRACE_MARKER_ID = 1u;
static const uint32_t TRACE_RECORD_GENERATION = 1u;
static const uint32_t TRACE_MARKER_PERIOD_MS = 200u;
static const uint32_t TRACE_METADATA_PERIOD_TICKS = 5000u; /* 1 kHz clock. */
/*******************************************************************************
 * kymoTraceConfig
 ******************************************************************************/
KymoEndpointTraceConfig kymoTraceConfig(void)
{
    KymoEndpointTraceConfig config = {};
    config.recorder.clock_ticks = kymoPortClockTicks;
    config.recorder.session_id = TRACE_SESSION_ID;
    config.recorder.clock_domain_id = TRACE_CLOCK_DOMAIN_ID;
    config.metadata = exampleTraceMetadata;
    config.metadata_length = sizeof(exampleTraceMetadata) - 1u;
    config.dictionary = exampleTraceDictionary;
    config.dictionary_length = sizeof(exampleTraceDictionary) - 1u;
    config.generation = TRACE_RECORD_GENERATION;
    config.metadata_period_ticks = TRACE_METADATA_PERIOD_TICKS;
    return config;
}
/*******************************************************************************
 * KymoApp_TraceInit
 ******************************************************************************/
KymoStatus KymoApp_TraceInit(const KymoEndpointTraceConfig *config)
{
    KymoStatus status = KymoEndpointTrace_Init(kymoEndpoint(), &sFeature, config);
    return status;
}
/*******************************************************************************
 * KymoApp_TraceHeartbeat
 ******************************************************************************/
void KymoApp_TraceHeartbeat(void)
{
    static uint32_t lastMarkerMs = 0u;
    uint32_t nowMs = kymoPortClockMs(nullptr);
    if ((KymoEndpointTrace_IsReady(&sFeature) != 0u) &&
        (static_cast<uint32_t>(nowMs - lastMarkerMs) >= TRACE_MARKER_PERIOD_MS)) {
        lastMarkerMs = nowMs;
        KYMO_ENDPOINT_TRACE_MARK(&sFeature, TRACE_CONTEXT_ID, TRACE_MARKER_ID, nowMs);
    }
}

#endif /* enabled feature */
