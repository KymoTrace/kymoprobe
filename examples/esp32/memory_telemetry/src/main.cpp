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
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include "kymo_memory.h"
#include <esp_heap_caps.h>

static const uint32_t MEMORY_BAUD = 115200u;
static const uint32_t SENSOR_STACK_BYTES = 2048u;
static const uint32_t PARSER_STACK_BYTES = 4096u;
static const uint32_t LOOP_STACK_BYTES = 8192u;  // Arduino-ESP32 loop task (CONFIG_ARDUINO_LOOP_STACK_SIZE)
static const uint32_t PARSER_MAX_DEPTH = 24u;    // ~110 bytes per level, leaves ~1 KiB reserve
static const size_t ENTRY_COUNT = 5u;
static const size_t TASK_COUNT = 3u;

TaskHandle_t taskHandles[TASK_COUNT] = {};
static volatile uint32_t parserDepth = 0u;
static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;
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

/** @brief Send one DATA point with the device time.
 * @param[in] point Channel and value. @param[in] nowMs Device time. */

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
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointMemoryConfig config = kymoMemoryConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = kymoMemoryInit(&core.endpoint, &config); }
}

/** @brief Send due MEMORY_INFO frames and samples. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    if ((status == KYMO_OK) || (status == KYMO_IDLE) || (status == KYMO_BUSY) || (status == KYMO_BAD_SAMPLE)) {
        status = KymoEndpoint_Main(&core.endpoint);
    }
    vTaskDelay(pdMS_TO_TICKS(1u));
}
