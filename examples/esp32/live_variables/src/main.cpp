/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief Live variables: KymoStudio samples variables it picked from the ELF file.
 * @details The firmware has a few ordinary globals that change over time. It does not
 * send them itself. KymoStudio loads this firmware's ELF file, the user ticks
 * variables, and KymoStudio asks the device over the same UART to sample them
 * (KymoWatch, KymoCore 7.3). The device reads each chosen variable at the requested
 * period and sends it as a timestamped DATA frame.
 *
 * Only the DRAM of .data and .bss (linker symbols _data_start to _bss_end) may be
 * read; every request outside it is refused. The build ID is the start of the ELF
 * file's SHA-256 that esptool stores in the app description (espressif32 6.12
 * passes --elf-sha256-offset). Without it the ID is empty and KymoStudio warns.
 * @note Arduino allocates SDK resources during Serial initialization only. */
#include <Arduino.h>
#include <KymoCore.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include "kymo_watch.h"
#include <math.h>

static const uint32_t BAUD = 115200u;
static const uint32_t STEP_PERIOD_MS = 10u;

/* Variables to pick in KymoStudio. volatile keeps them in memory with their own address. */
volatile uint32_t loopCounter = 0u;
volatile float wave = 0.0f;
volatile double uptimeSeconds = 0.0;
volatile bool alarmActive = false;

struct MotorState {
    int16_t rpm;
    uint8_t mode;
    float current;
};
volatile MotorState motor = { 0, 0u, 0.0f };

static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;
static uint32_t lastStepMs = 0u;

/** @brief Encode and send one sample with the device time.
 * @param[in] point Sample from KymoWatch. @param[in] nowMs Device time. */

/** @brief Read host requests, send replies and every due live-variable sample.
 * @param[in] nowMs Device time. */

/** @brief Let the variables change: a counter, a sine, a slow motor model. */
/*******************************************************************************
 * step
 ******************************************************************************/
static void step(uint32_t nowMs)
{
    const float t = static_cast<float>(nowMs) / 1000.0f;
    loopCounter = loopCounter + 1u;
    wave = sinf(2.0f * static_cast<float>(M_PI) * 0.5f * t);
    uptimeSeconds = static_cast<double>(nowMs) / 1000.0;
    motor.mode = static_cast<uint8_t>((nowMs / 5000u) % 3u);
    const float target = (motor.mode == 0u) ? 0.0f : ((motor.mode == 1u) ? 1500.0f : 3000.0f);
    const float rpm = static_cast<float>(motor.rpm) + ((target - static_cast<float>(motor.rpm)) * 0.02f);
    motor.rpm = static_cast<int16_t>(rpm);
    motor.current = 0.2f + (rpm / 1000.0f);
    alarmActive = rpm > 2800.0f;
}

/** @brief Fill the build ID from the app description; all zeros means unknown.
 * @return Number of build ID bytes (0 or 16). */

/** @brief Start the UART and allow reads of .data and .bss. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.setTxBufferSize(1024u);
    Serial.begin(BAUD);
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointWatchConfig config = kymoWatchConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = kymoWatchInit(&core.endpoint, &config); }
    lastStepMs = millis();
}

/** @brief Live variables every pass, the simulation at 100 Hz. */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    const uint32_t nowMs = millis();
    if ((status == KYMO_OK) || (status == KYMO_IDLE) || (status == KYMO_BUSY) || (status == KYMO_BAD_SAMPLE)) {
        status = KymoEndpoint_Main(&core.endpoint);
    }
    if ((nowMs - lastStepMs) >= STEP_PERIOD_MS) {
        lastStepMs += STEP_PERIOD_MS;
        step(nowMs);
    }
}
