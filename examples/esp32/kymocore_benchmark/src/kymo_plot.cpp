/* SPDX-License-Identifier: MIT */
/** @file kymo_plot.cpp @brief Isolated plot operations under measurement. */
#include "kymo_plot.h"
#include "benchmark.h"
#include <KymoCore.h>
#include <xtensa/core-macros.h>
/*******************************************************************************
 * benchEncode
 ******************************************************************************/
uint32_t benchEncode(void)
{
    uint8_t frame[KYMO_FRAME_MAX_SIZE] = {};
    KymoDataPoint point = {};
    point.id = 1u;
    point.flags = KYMO_FLAG_TIMESTAMP;
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        point.value = static_cast<float>(run);
        point.timestamp_ms = run;
        sink += static_cast<uint32_t>(KymoProtocol_Encode(&point, frame, sizeof(frame)));
    }
    return XTHAL_GET_CCOUNT() - start;
}
