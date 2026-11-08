/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief Task stack peaks and heap levels as named curves (KymoMemory).
 * @details Three FreeRTOS tasks and the heap are listed as KymoMemory entries. The read
 * callback turns uxTaskGetStackHighWaterMark (bytes on ESP-IDF) into peak stack use and
 * asks heap_caps for the free and the lowest free internal heap. KymoMemory sends a
 * MEMORY_INFO frame per entry on start and every 5 s, so KymoStudio names the curves
 * after the tasks and knows each total size. The parser task deepens a recursion by one
 * level per second up to a cap, so its peak climbs in steps.
 * The watermark scan costs time proportional to the unused stack, so sampling runs at
 * 2 Hz in the loop task, never in an ISR. */
#include <Arduino.h>
#include <KymoCore.h>
#include <esp_heap_caps.h>

static const uint32_t MEMORY_BAUD = 115200u;
static const uint32_t SENSOR_STACK_BYTES = 2048u;
static const uint32_t PARSER_STACK_BYTES = 4096u;
static const uint32_t LOOP_STACK_BYTES = 8192u;  // Arduino-ESP32 loop task (CONFIG_ARDUINO_LOOP_STACK_SIZE)
static const uint32_t PARSER_MAX_DEPTH = 24u;    // ~110 bytes per level, leaves ~1 KiB reserve
static const size_t ENTRY_COUNT = 5u;
static const size_t TASK_COUNT = 3u;

static TaskHandle_t taskHandles[TASK_COUNT] = {};
static volatile uint32_t parserDepth = 0u;
static KymoMemory memory;
static KymoMemoryEntry entries[ENTRY_COUNT] = {
    {"loop", LOOP_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 220u},
    {"sensor", SENSOR_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 221u},
    {"parser", PARSER_STACK_BYTES, KYMO_MEMORY_STACK_PEAK, 222u},
    {"internal", 0u, KYMO_MEMORY_HEAP_FREE, 223u},
    {"internal", 0u, KYMO_MEMORY_HEAP_MIN_FREE, 224u},
};

/** @brief Use one stack frame per level; volatile keeps the frame from being optimized away.
 * @param[in] depth Remaining recursion levels. @return Checksum that depends on every frame. */
/*******************************************************************************
 * consumeStack
 ******************************************************************************/
static uint32_t __attribute__((noinline)) consumeStack(uint32_t depth)
{
    volatile uint8_t frame[64] = {};
    uint32_t sum = depth;
    frame[0] = static_cast<uint8_t>(depth);
    if (depth > 1u) {
        sum += consumeStack(depth - 1u);
    }
    frame[63] = static_cast<uint8_t>(sum);
    return sum + frame[0] + frame[63];
}

/** @brief Light, constant stack use: a 100-Hz pseudo sensor. @param[in] arg Unused. */
/*******************************************************************************
 * sensorTask
 ******************************************************************************/
static void sensorTask(void *arg)
{
    volatile float filtered = 0.0f;
    (void)arg;
    for (;;) {
        filtered = (0.9f * filtered) + (0.1f * static_cast<float>(micros() % 1000u));
        vTaskDelay(pdMS_TO_TICKS(10u));
    }
}

/** @brief Stack use that grows by one recursion level per second up to the cap. @param[in] arg Unused. */
/*******************************************************************************
 * parserTask
 ******************************************************************************/
static void parserTask(void *arg)
{
    volatile uint32_t checksum = 0u;
    (void)arg;
    for (;;) {
        if (parserDepth < PARSER_MAX_DEPTH) {
            parserDepth = parserDepth + 1u;
        }
        checksum = consumeStack(parserDepth);
        vTaskDelay(pdMS_TO_TICKS(1000u));
    }
}

/** @brief KymoMemory callback: peak stack use or heap level of one entry in bytes.
 * @param[in] context Unused. @param[in] index Entry index. @param[out] bytes Value.
 * @return 1 when the value is valid. */
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

/** @brief Send one DATA point with the device time.
 * @param[in] point Channel and value. @param[in] nowMs Device time. */
/*******************************************************************************
 * sendPoint
 ******************************************************************************/
static void sendPoint(KymoDataPoint point, uint32_t nowMs)
{
    uint8_t frame[KYMO_FRAME_MAX_SIZE] = {};
    size_t length = 0u;
    point.flags = KYMO_FLAG_TIMESTAMP;
    point.timestamp_ms = nowMs;
    length = KymoProtocol_Encode(&point, frame, sizeof(frame));
    if (length != 0u) {
        (void)Serial.write(frame, length);
    }
}

/** @brief Start the UART, the worker tasks and KymoMemory; setup() runs in the loop task. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.setTxBufferSize(ENTRY_COUNT * KYMO_MEMORY_FRAME_MAX_SIZE * 4u);
    Serial.begin(MEMORY_BAUD);
    taskHandles[0] = xTaskGetCurrentTaskHandle();
    (void)xTaskCreate(sensorTask, "sensor", SENSOR_STACK_BYTES, nullptr, 2u, &taskHandles[1]);
    (void)xTaskCreate(parserTask, "parser", PARSER_STACK_BYTES, nullptr, 2u, &taskHandles[2]);
    entries[3].total_bytes = static_cast<uint32_t>(heap_caps_get_total_size(MALLOC_CAP_INTERNAL));
    entries[4].total_bytes = entries[3].total_bytes;
    const KymoMemoryConfig config = {entries, ENTRY_COUNT, 500u, 5000u, readMemory, nullptr};
    (void)KymoMemory_Init(&memory, &config);
}

/** @brief Send due MEMORY_INFO frames and samples. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    const uint32_t nowMs = millis();
    uint8_t frame[KYMO_MEMORY_FRAME_MAX_SIZE] = {};
    size_t length = KymoMemory_NextInfo(&memory, nowMs, frame, sizeof(frame));
    KymoDataPoint point = {};
    while (length != 0u) {
        (void)Serial.write(frame, length);
        length = KymoMemory_NextInfo(&memory, nowMs, frame, sizeof(frame));
    }
    while (KymoMemory_NextSample(&memory, nowMs, &point) == KYMO_OK) {
        sendPoint(point, nowMs);
    }
    vTaskDelay(pdMS_TO_TICKS(10u));
}
