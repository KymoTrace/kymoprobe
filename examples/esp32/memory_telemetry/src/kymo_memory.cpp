/* SPDX-License-Identifier: MIT */
/** @file kymo_memory.cpp @brief Task/heap measurements and descriptions. */
#include "kymo_memory.h"
#include <Arduino.h>
#include <esp_heap_caps.h>
static const uint32_t SENSOR_STACK_BYTES = 2048u;
static const uint32_t PARSER_STACK_BYTES = 4096u;
static const uint32_t LOOP_STACK_BYTES = 8192u;
static const size_t ENTRY_COUNT = 5u;
static const size_t TASK_COUNT = 3u;
extern TaskHandle_t taskHandles[TASK_COUNT];
static KymoMemoryEntry entries[ENTRY_COUNT] = {
    {"loop", LOOP_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 220u},
    {"sensor", SENSOR_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 221u},
    {"parser", PARSER_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 222u},
    {"internal", 0u, KYMO_MEMORY_HEAP_FREE, 223u},
    {"internal", 0u, KYMO_MEMORY_HEAP_MIN_FREE, 224u},
};

/*******************************************************************************
 * readMemory
 ******************************************************************************/
static uint8_t readMemory(void *context, size_t index, uint32_t *bytes)
{
    uint8_t valid = 0u;
    (void)context;
    if ((index < TASK_COUNT) && (taskHandles[index] != nullptr)) {
        // ESP-IDF reports the watermark in bytes; vanilla FreeRTOS needs * sizeof(StackType_t).
        *bytes = entries[index].total_bytes -
                 static_cast<uint32_t>(uxTaskGetStackHighWaterMark(taskHandles[index]));
        valid = 1u;
    } else if (index == 3u) {
        *bytes = static_cast<uint32_t>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
        valid = 1u;
    } else if (index == 4u) {
        *bytes = static_cast<uint32_t>(heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL));
        valid = 1u;
    } else {
        /* Task not created: skip the sample. */
    }
    return valid;
}
/*******************************************************************************
 * kymoMemoryConfig
 ******************************************************************************/
KymoEndpointMemoryConfig kymoMemoryConfig(void)
{
    KymoEndpointMemoryConfig config = {};
    entries[3].total_bytes = static_cast<uint32_t>(heap_caps_get_total_size(MALLOC_CAP_INTERNAL));
    entries[4].total_bytes = entries[3].total_bytes;
    config.entries = entries;
    config.count = ENTRY_COUNT;
    config.period_ms = 500u;
    config.info_period_ms = 5000u;
    config.read = readMemory;
    return config;
}

static KymoEndpointMemory feature;
/*******************************************************************************
 * kymoMemoryInit
 ******************************************************************************/
KymoStatus kymoMemoryInit(KymoEndpoint *endpoint, const KymoEndpointMemoryConfig *config)
{
    KymoStatus status = KymoEndpointMemory_Init(endpoint, &feature, config);
    return status;
}
