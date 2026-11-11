/* SPDX-License-Identifier: MIT */
/** @file kymo_trace.h @brief Trace-only configuration and instrumentation access.
 * @details Initialize once before starting producers. Cache the recorder before
 * tasks/ISRs start; never stop/reinitialize while they are active. Port callbacks
 * serialize recorder data, while Endpoint Main remains owned by the main loop.
 * @par Usage Configure, initialize, then record through kymoTraceRecorder(). */
#pragma once
#include <endpoint/features/kymo_trace.h>
#include <kymo_trace_ids.h>
/** @brief Build persistent metadata and recorder bindings. @return Trace config. */
KymoEndpointTraceConfig kymoTraceConfig(void);
/** @brief Initialize feature-owned storage.
 * @param[in,out] endpoint Initialized base. @param[in] config Trace settings.
 * @return Initialization status. */
KymoStatus kymoTraceInit(KymoEndpoint *endpoint, const KymoEndpointTraceConfig *config);
/** @brief Get the pointer cached before producer startup; no Endpoint reads.
 * @return Recorder, or nullptr before successful Init. */
KymoTraceRecorder *kymoTraceRecorder(void);
