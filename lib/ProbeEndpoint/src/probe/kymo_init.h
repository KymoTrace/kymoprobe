/* SPDX-License-Identifier: MIT */
/** @file kymo_init.h @brief Application-owned Endpoint storage, without feature settings.
 * @details One serialized owner. Instances and referenced callback contexts must
 * persist. The port must release borrowed bytes before ending this object's lifetime.
 * @par Usage Init the base, initialize selected features, then call Endpoint Main. */
#pragma once
#include <endpoint/kymo_endpoint.h>
#include <telemetry/kymo_protocol.h>
#if KYMO_EFFECTIVE_PARAMETERS
#include <control/kymo_param.h>
#endif
#if KYMO_EFFECTIVE_WATCH
#include <control/kymo_watch.h>
#endif
#if KYMO_EFFECTIVE_MEMORY
#include <telemetry/kymo_memory.h>
#endif
#if KYMO_EFFECTIVE_TRACE
#include <trace/kymo_trace_protocol.h>
#endif
#if KYMO_EFFECTIVE_ENDPOINT
/** @brief Largest selected frame; computed without unexplained byte capacities. */
#define PROBE_MAX_FRAME(a, b) ((a) > (b) ? (a) : (b))
/** @brief Frame capacities are zero for disabled features. */
enum {
#if KYMO_EFFECTIVE_PARAMETERS
    PROBE_PARAM_CAPACITY = KYMO_PARAM_FRAME_MAX_SIZE,
#else
    PROBE_PARAM_CAPACITY = 0u,
#endif
#if KYMO_EFFECTIVE_WATCH
    PROBE_WATCH_CAPACITY = KYMO_WATCH_FRAME_MAX_SIZE,
#else
    PROBE_WATCH_CAPACITY = 0u,
#endif
#if KYMO_EFFECTIVE_MEMORY
    PROBE_MEMORY_CAPACITY = KYMO_MEMORY_FRAME_MAX_SIZE,
#else
    PROBE_MEMORY_CAPACITY = 0u,
#endif
#if KYMO_EFFECTIVE_TRACE
    PROBE_TRACE_CAPACITY = KYMO_TRACE_FRAME_MAX,
#else
    PROBE_TRACE_CAPACITY = 0u,
#endif
    PROBE_TX_CAPACITY = PROBE_MAX_FRAME(KYMO_FRAME_MAX_SIZE,
        PROBE_MAX_FRAME(PROBE_MAX_FRAME(PROBE_PARAM_CAPACITY, PROBE_WATCH_CAPACITY),
            PROBE_MAX_FRAME(PROBE_MEMORY_CAPACITY, PROBE_TRACE_CAPACITY)))
};
/** @brief Bounded incoming bytes per processing call. */
enum { PROBE_RX_BUDGET = 32u };
/** @brief Base and owned transport buffers; no feature instances.
 * @par Usage Static zero-initialized storage, initialized once. */
typedef struct {
    KymoEndpoint endpoint; /**< Shared coordinator. */
    uint8_t tx[PROBE_TX_CAPACITY]; /**< Persistent borrowed/copying TX storage. */
    uint8_t rx[PROBE_RX_BUDGET]; /**< Bounded control input. */
} ProbeCore;
#ifdef __cplusplus
extern "C" {
#endif
/** @brief Initialize base storage with project-supplied port callbacks.
 * @param[in,out] core Persistent zero-initialized storage.
 * @param[in] port Base config; buffers are supplied by core.
 * @return Initialization result. @par Usage Before feature Init calls. */
KymoStatus probeCoreInit(ProbeCore *core, const KymoEndpointConfig *port);
#ifdef __cplusplus
}
#endif
#endif
