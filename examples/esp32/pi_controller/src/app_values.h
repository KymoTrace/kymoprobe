/* SPDX-License-Identifier: MIT */
/** @file app_values.h @brief Controller values shared with serialized telemetry bindings. */
#pragma once
#include <stdint.h>
extern float kp, ki, setpoint, temperature, heater;
/** @brief Maximum whole Endpoint processing time since last diagnostic sample, in us. */
extern uint32_t backChannelMaxUs;
