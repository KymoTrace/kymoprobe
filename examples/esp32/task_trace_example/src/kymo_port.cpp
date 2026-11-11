/** @file kymo_port.cpp @brief UART, clock and synchronization adapter. */
#include <KymoCore.h>
#if KYMO_EFFECTIVE_TRACE
#include "kymo_port.h"
#include <Arduino.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <kymo_trace_ids.h>
#include <probe/kymo_port.h>
#include "app_tasks.h"
static portMUX_TYPE sTraceLock = portMUX_INITIALIZER_UNLOCKED;
static uint64_t sSessionId;
static const uint32_t TASK_CONTEXTS[TASK_COUNT] = {
    KYMO_CONTEXT_FAST_WORKER, KYMO_CONTEXT_SLOW_WORKER,
    KYMO_CONTEXT_BACKGROUND_WORKER, KYMO_CONTEXT_ALARM_PRODUCER,
    KYMO_CONTEXT_IRQ_CONSUMER, KYMO_CONTEXT_PIPELINE,
    KYMO_CONTEXT_COMPUTE_LOW, KYMO_CONTEXT_CONTROL_HIGH
};

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
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    uint32_t result = 0u;
    (void)user;
    if (current == sLoopTask) {
        result = KYMO_CONTEXT_MAIN;
    }
    for (uint32_t index = 0u; index < TASK_COUNT; ++index) {
        if ((sTaskHandles[index] != nullptr) && (current == sTaskHandles[index])) {
            result = TASK_CONTEXTS[index];
        }
    }
    return result;
}
/*******************************************************************************
 * traceCriticalEnter
 ******************************************************************************/
extern "C" void traceCriticalEnter(void *user)
{
    (void)user;
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&sTraceLock);
    } else {
        portENTER_CRITICAL(&sTraceLock);
    }
}
/*******************************************************************************
 * traceCriticalExit
 ******************************************************************************/
extern "C" void traceCriticalExit(void *user)
{
    (void)user;
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&sTraceLock);
    } else {
        portEXIT_CRITICAL(&sTraceLock);
    }
}
/*******************************************************************************
 * kymo_trace_session_id
 ******************************************************************************/
extern "C" uint64_t kymo_trace_session_id(void)
{
    return sSessionId;
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
    sSessionId = (static_cast<uint64_t>(esp_random()) << 32u) | esp_random();
    if (sSessionId == 0u) { sSessionId = 1u; }
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
