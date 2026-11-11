/** @file app_tasks.h @brief Stable application task identities for the trace port. */
#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Config.h"
/** @brief Handles assigned before producer notification; unchanged thereafter. */
extern TaskHandle_t sTaskHandles[TASK_COUNT];
/** @brief Main loop task handle assigned during setup. */
extern TaskHandle_t sLoopTask;
