/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Y/XY/XYZ channels and codec self-check. */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_DEMO_MODE == KYMO_DEMO_FEATURES && KYMO_ENABLED
#include "kymo_plot.h"
#include <probe/kymo_port.h>
#include <math.h>
static bool isDecoderValid = false;
static const uint32_t SAMPLE_PERIOD_MS = 20u;
static const KymoChannel channels[] = {
    {SAMPLE_PERIOD_MS, 0u, KYMO_FLAG_TIMESTAMP},
    {SAMPLE_PERIOD_MS, 1u, KYMO_FLAG_X | KYMO_FLAG_TIMESTAMP},
    {SAMPLE_PERIOD_MS, 2u, KYMO_FLAG_X | KYMO_FLAG_Z | KYMO_FLAG_TIMESTAMP},
    {SAMPLE_PERIOD_MS, 3u, KYMO_FLAG_TIMESTAMP}
};
/*******************************************************************************
 * verifyFeatureDecoder
 ******************************************************************************/
static bool verifyFeatureDecoder(void)
{
    KymoDataPoint point = {};
    KymoDataPoint decoded = {};
    uint8_t frame[KYMO_FRAME_MAX_SIZE] = {};
    size_t length = 0u;
    bool isValid = false;
    point.id = 7u;
    point.flags = KYMO_FLAG_X | KYMO_FLAG_Z | KYMO_FLAG_TIMESTAMP;
    point.x = 1.0f;
    point.value = -0.5f;
    point.z = 0.25f;
    point.timestamp_ms = 1234u;
    length = KymoProtocol_Encode(&point, frame, sizeof(frame));
    if (length != 0u) {
        isValid = (KymoProtocol_Decode(frame, length, &decoded) == KYMO_OK) &&
            (decoded.id == point.id) && (decoded.flags == point.flags) &&
            (decoded.timestamp_ms == point.timestamp_ms) &&
            (fabsf(decoded.x - point.x) < 0.0001f) &&
            (fabsf(decoded.value - point.value) < 0.0001f) &&
            (fabsf(decoded.z - point.z) < 0.0001f);
        frame[length - 1u] ^= 0x01u;
        isValid = (KymoProtocol_Decode(frame, length, &decoded) != KYMO_OK) && isValid;
    }
    return isValid;
}
/*******************************************************************************
 * readFeatureSample
 ******************************************************************************/
static uint8_t readFeatureSample(void *user, uint8_t id, KymoSample *sample)
{
    uint8_t valid = 0u;
    const float phase = static_cast<float>(probeSerialClock(nullptr) % 2000u) / 2000.0f;
    (void)user;
    if ((sample != nullptr) && (id < KYMO_COUNT_OF(channels))) {
        sample->value = id == 3u ? (isDecoderValid ? 1.0f : 0.0f) : sinf(phase * 6.283185307f);
        sample->x = phase;
        sample->z = 1.0f - phase;
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
    isDecoderValid = verifyFeatureDecoder();
    config.channels = channels;
    config.channel_count = KYMO_COUNT_OF(channels);
    config.sample = readFeatureSample;
    return config;
}
#endif
