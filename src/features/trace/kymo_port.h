/** @file kymo_port.h @brief Hardware callbacks for Trace and the shared Endpoint. */
#pragma once
#include <endpoint/kymo_endpoint.h>
#include <trace/kymo_trace.h>
/** @brief Initialize the boot session before any producers start. */
void kymoTracePortInit(void);
/** @brief Build serial transport settings. @return Port configuration. */
KymoEndpointConfig kymoPortConfig(void);
/** @brief Build synchronized recorder callbacks. @return Recorder settings. */
KymoTraceConfig kymoRecorderConfig(void);
/** @brief Read the hardware clock. @param[in] user Unused. @return Microseconds. */
extern "C" uint64_t traceClockTicks(void *user);
