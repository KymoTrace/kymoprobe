/* SPDX-License-Identifier: MIT */
/** @file kymo_init.cpp
 * @brief Common Endpoint, frame buffer and port binding.
 */
#include "kymo_init.h"
#include "kymo_port.h"
#include "detail/kymo_internal.h"
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

/** @brief Combine frame capacities at compile time. */
/*******************************************************************************
 * largerFrame
 ******************************************************************************/
static constexpr size_t largerFrame(size_t first, size_t second)
{
    return first > second ? first : second;
}
/* Only transport sizing here; feature settings remain in their own files. */
static const size_t PARAM_CAPACITY =
#if KYMO_EFFECTIVE_PARAMETERS
    KYMO_PARAM_FRAME_MAX_SIZE;
#else
    0u;
#endif
static const size_t WATCH_CAPACITY =
#if KYMO_EFFECTIVE_WATCH
    KYMO_WATCH_FRAME_MAX_SIZE;
#else
    0u;
#endif
static const size_t MEMORY_CAPACITY =
#if KYMO_EFFECTIVE_MEMORY
    KYMO_MEMORY_FRAME_MAX_SIZE;
#else
    0u;
#endif
static const size_t TRACE_CAPACITY =
#if KYMO_EFFECTIVE_TRACE
    KYMO_TRACE_FRAME_MAX;
#else
    0u;
#endif
static const size_t TX_CAPACITY = largerFrame(KYMO_FRAME_MAX_SIZE,
    largerFrame(largerFrame(PARAM_CAPACITY, WATCH_CAPACITY),
        largerFrame(MEMORY_CAPACITY, TRACE_CAPACITY)));
static KymoEndpoint sEndpoint;
static uint8_t sTxBuffer[TX_CAPACITY];
#if KYMO_EFFECTIVE_PARAMETERS || KYMO_EFFECTIVE_WATCH
static const size_t RX_BUDGET_BYTES = 16u;
static uint8_t sRxBuffer[RX_BUDGET_BYTES];
#endif

/*******************************************************************************
 * kymoEndpoint
 ******************************************************************************/
KymoEndpoint *kymoEndpoint(void)
{
    return &sEndpoint;
}
/*******************************************************************************
 * kymoCoreConfig
 ******************************************************************************/
KymoEndpointConfig kymoCoreConfig(void)
{
    KymoEndpointConfig config = {};
    config.tx.buffer = sTxBuffer;
    config.tx.capacity = sizeof(sTxBuffer);
    config.tx.write = kymoPortWrite;
    config.clock_ms = kymoPortClockMs;
#if KYMO_EFFECTIVE_PARAMETERS || KYMO_EFFECTIVE_WATCH
    config.read = kymoPortRead;
    config.rx_buffer = sRxBuffer;
    config.rx_capacity = sizeof(sRxBuffer);
#endif
    /* Serial copies immediately, so no borrowed-buffer busy callback is needed. */
    return config;
}
/*******************************************************************************
 * KymoApp_Init
 ******************************************************************************/
KymoStatus KymoApp_Init(const KymoEndpointConfig *config)
{
    KymoStatus status = KymoEndpoint_Init(&sEndpoint, config);
    return status;
}
/*******************************************************************************
 * KymoApp_Main
 ******************************************************************************/
KymoStatus KymoApp_Main(void)
{
    KymoStatus status = KymoEndpoint_Main(&sEndpoint);
    return status;
}
/*******************************************************************************
 * KymoApp_RequestStop
 ******************************************************************************/
KymoStatus KymoApp_RequestStop(void)
{
    KymoStatus status = KymoEndpoint_RequestStop(&sEndpoint);
    return status;
}
