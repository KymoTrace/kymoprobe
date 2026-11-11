/** @file kymo_port.cpp @brief UART, clock and synchronization adapter. */
#include "demo_selection.h"
#include <KymoCore.h>
#if ((KYMO_DEMO_MODE == KYMO_DEMO_TRACE) || (KYMO_DEMO_MODE == KYMO_DEMO_TRACE_FLOW)) && KYMO_EFFECTIVE_TRACE
#include "kymo_port.h"
#include <Arduino.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <kymo_trace_ids.h>
#include <probe/kymo_port.h>
static portMUX_TYPE traceMuxLock = portMUX_INITIALIZER_UNLOCKED;
static uint64_t traceSessionId;
/*******************************************************************************
 * traceClockTicks
 ******************************************************************************/
extern "C" uint64_t traceClockTicks(void *user)
{
    uint64_t result = static_cast<uint64_t>(esp_timer_get_time());
    (void)user;
    return result;
}
/*******************************************************************************
 * traceCoreId
 ******************************************************************************/
extern "C" uint8_t traceCoreId(void *user)
{
    uint8_t result = static_cast<uint8_t>(xPortGetCoreID());
    (void)user;
    return result;
}
/*******************************************************************************
 * traceCurrentContext
 ******************************************************************************/
extern "C" uint32_t traceCurrentContext(void *user)
{
    uint32_t result = KYMO_CONTEXT_MAIN;
    (void)user;
    return result;
}
/*******************************************************************************
 * traceCriticalEnter
 ******************************************************************************/
extern "C" void traceCriticalEnter(void *user)
{
    (void)user;
    portENTER_CRITICAL(&traceMuxLock);
}
/*******************************************************************************
 * traceCriticalExit
 ******************************************************************************/
extern "C" void traceCriticalExit(void *user)
{
    (void)user;
    portEXIT_CRITICAL(&traceMuxLock);
}
/*******************************************************************************
 * kymo_trace_session_id
 ******************************************************************************/
extern "C" uint64_t kymo_trace_session_id(void)
{
    return traceSessionId;
}
/*******************************************************************************
 * traceIsBusy
 ******************************************************************************/
extern "C" uint8_t traceIsBusy(void *user)
{
    uint8_t result = Serial.availableForWrite() <= 0 ? 1u : 0u;
    (void)user;
    return result;
}
/*******************************************************************************
 * traceWriteFrame
 ******************************************************************************/
extern "C" size_t traceWriteFrame(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    const int available = Serial.availableForWrite();
    (void)user;
    if ((bytes != nullptr) && (available > 0)) {
        const size_t capacity = static_cast<size_t>(available);
        accepted = Serial.write(bytes, length < capacity ? length : capacity);
    }
    return accepted;
}
/*******************************************************************************
 * kymoTracePortInit
 ******************************************************************************/
void kymoTracePortInit(void)
{
    traceSessionId = (static_cast<uint64_t>(esp_random()) << 32u) | esp_random();
    if (traceSessionId == 0u) { traceSessionId = 1u; }
}
/*******************************************************************************
 * kymoPortConfig
 ******************************************************************************/
KymoEndpointConfig kymoPortConfig(void)
{
    KymoEndpointConfig config = probeSerialConfig();
    config.tx.write = traceWriteFrame;
    return config;
}
/*******************************************************************************
 * kymoRecorderConfig
 ******************************************************************************/
KymoTraceConfig kymoRecorderConfig(void)
{
    KymoTraceConfig config = {};
    config.clock_ticks = traceClockTicks;
    config.core_id = traceCoreId;
    config.current_context = traceCurrentContext;
    config.critical_enter = traceCriticalEnter;
    config.critical_exit = traceCriticalExit;
    config.session_id = kymo_trace_session_id();
    config.clock_domain_id = 1u;
    return config;
}
#endif
