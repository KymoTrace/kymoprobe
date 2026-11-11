/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file main.cpp @brief Two 100-Hz signals as JSON text lines without timestamps.
 * @details Test firmware for KymoStudio's reception-time path (test case P2):
 * each line is {"id":n,"value":v}, so Studio must fall back to reception time
 * and refuse time-dependent analysis. No KymoCore framing, no CRC.
 * @note Arduino allocates SDK resources during Serial initialization only. */
#include <Arduino.h>
#include "kymo_plot.h"
#include "kymo_port.h"
#include <stdio.h>

static const uint32_t TEXT_BAUD = 115200u;
static const uint32_t SAMPLE_PERIOD_MS = 10u;
static uint32_t lastSampleMs = 0u;
static uint32_t sampleIndex = 0u;

/** @brief Send one JSON measurement line.
 * @param[in] id Signal identifier. @param[in] value Measured value. */

/** @brief Initialize the board UART. */
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup(void)
{
    Serial.begin(TEXT_BAUD);
    lastSampleMs = millis();
}

/** @brief Every 10 ms send a 1-Hz sine (id 0) and a 0.5-Hz sawtooth (id 1). */
/*******************************************************************************
 * loop
 ******************************************************************************/
void loop(void)
{
    const uint32_t nowMs = millis();
    TextSamples sample = {};
    if ((nowMs - lastSampleMs) >= SAMPLE_PERIOD_MS) {
        lastSampleMs += SAMPLE_PERIOD_MS;
        sample = textPlotSample(sampleIndex);
        textPortSend(0u, sample.sine);
        textPortSend(1u, sample.saw);
        ++sampleIndex;
    }
}
