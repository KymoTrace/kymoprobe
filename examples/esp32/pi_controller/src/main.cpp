/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief PI temperature controller whose gains and setpoint KymoStudio tunes live.
 * @details A simulated heater (first-order thermal model) is controlled by a PI
 * controller at 100 Hz. Temperature, heater output and setpoint go out at 50 Hz as
 * timestamped Kymo DATA frames. kp, ki and setpoint are registered with KymoParam:
 * KymoStudio asks for the list when it connects, shows them in its parameter
 * panel and writes new values over the same UART. Every accepted or refused
 * write is answered with a readback of the value now in force.
 * Signal 3 reports the longest back-channel handling time per report period, so
 * the cost of the back channel is visible next to the control loop.
 * @note Arduino allocates SDK resources during Serial initialization only. */
#include <Arduino.h>
#include <KymoCore.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include "kymo_plot.h"
#include "kymo_param.h"
#include "app_values.h"

static const uint32_t BAUD = 115200u;
static const uint32_t CONTROL_PERIOD_MS = 10u;

static const float AMBIENT_C = 20.0f;
static const float HEATER_GAIN_C = 80.0f;   // steady-state rise at full heater power
static const float TIME_CONSTANT_S = 8.0f;
static const float DT_S = 0.01f;

/* Tunable from KymoStudio; names come from the variable names. */
float kp = 0.20f;
float ki = 0.02f;
float setpoint = 45.0f;

static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;
float temperature = AMBIENT_C;
float heater = 0.0f;
static float integral = 0.0f;
static uint32_t lastControlMs = 0u;

uint32_t backChannelMaxUs = 0u;

/** @brief Encode and send one sample with the device time.
 * @param[in] id Signal identifier. @param[in] value Value. @param[in] nowMs Device time. */

/** @brief Read what the host sent, apply writes, answer list requests and readbacks.
 * @param[in] nowMs Device time for the readbacks. */

/** @brief One PI step with anti-windup and one step of the heater model. */
/*******************************************************************************
 * controlStep
 ******************************************************************************/
static void controlStep(void)
{
    const float error = setpoint - temperature;
    const float candidate = integral + (error * DT_S);
    float output = (kp * error) + (ki * candidate);
    if ((output > 0.0f) && (output < 1.0f)) {
        integral = candidate;  // integrate only while the heater is not saturated
    }
    output = constrain(output, 0.0f, 1.0f);
    heater = output;
    temperature += (((heater * HEATER_GAIN_C) - (temperature - AMBIENT_C)) / TIME_CONSTANT_S) * DT_S;
}

/** @brief Start the UART and register the parameter table. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.setTxBufferSize(1024u);
    Serial.begin(BAUD);
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointPlotConfig plotConfig = kymoPlotConfig();
    const KymoEndpointParamConfig paramConfig = kymoParamConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = kymoPlotInit(&core.endpoint, &plotConfig); }
    if (status == KYMO_OK) { status = kymoParamInit(&core.endpoint, &paramConfig); }
    lastControlMs = millis();

}

/** @brief Back channel every pass, control at 100 Hz, report at 50 Hz. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    const uint32_t nowMs = millis();

    if ((nowMs - lastControlMs) >= CONTROL_PERIOD_MS) {
        lastControlMs += CONTROL_PERIOD_MS;
        controlStep();
    }
    if ((status == KYMO_OK) || (status == KYMO_IDLE) || (status == KYMO_BUSY) || (status == KYMO_BAD_SAMPLE)) {
        const uint32_t startUs = micros();
        status = KymoEndpoint_Main(&core.endpoint);
        const uint32_t elapsedUs = micros() - startUs;
        if (elapsedUs > backChannelMaxUs) { backChannelMaxUs = elapsedUs; }
    }
}
