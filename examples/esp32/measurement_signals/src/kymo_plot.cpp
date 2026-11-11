/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Coherent synthetic grid with visible gaps under backpressure. */
#include "kymo_plot.h"
#include <probe/kymo_port.h>
#include "Waveforms.h"
static uint32_t epochMs;
static uint32_t gridMs;
static uint32_t completed;
static MeasurementSet_T measurement;
static KymoChannel channels[MEASUREMENT_CHANNEL_COUNT];
/*******************************************************************************
 * kymoPlotClock
 ******************************************************************************/
uint32_t kymoPlotClock(void *user)
{
    const uint32_t elapsed = probeSerialClock(nullptr) - epochMs;
    const uint32_t due = elapsed - gridMs;
    (void)user;
    if ((completed == MEASUREMENT_CHANNEL_COUNT) && (due >= SAMPLE_PERIOD_MS)) {
        gridMs += (due / SAMPLE_PERIOD_MS) * SAMPLE_PERIOD_MS;
        measurement = calculateMeasurementSet(gridMs);
        completed = 0u;
    }
    return gridMs;
}
/*******************************************************************************
 * readMeasurement
 ******************************************************************************/
static uint8_t readMeasurement(void *user, uint8_t id, KymoSample *sample)
{
    uint8_t valid = 0u;
    (void)user;
    if ((sample != nullptr) && (id < MEASUREMENT_CHANNEL_COUNT)) {
        sample->value = measurement.values[id];
        ++completed;
        valid = 1u;
    }
    return valid;
}
/*******************************************************************************
 * kymoPlotConfig
 ******************************************************************************/
KymoEndpointPlotConfig kymoPlotConfig(void)
{
    KymoEndpointPlotConfig config = {};
    epochMs = probeSerialClock(nullptr);
    gridMs = 0u;
    completed = 0u;
    measurement = calculateMeasurementSet(gridMs);
    for (uint32_t id = 0u; id < MEASUREMENT_CHANNEL_COUNT; ++id) {
        channels[id].id = static_cast<uint8_t>(id);
        channels[id].period_ms = SAMPLE_PERIOD_MS;
        channels[id].flags = KYMO_FLAG_TIMESTAMP;
    }
    config.channels = channels;
    config.channel_count = MEASUREMENT_CHANNEL_COUNT;
    config.sample = readMeasurement;
    return config;
}
