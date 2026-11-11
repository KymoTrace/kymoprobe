/* SPDX-License-Identifier: MIT */
/** @file kymo_trace.cpp @brief Isolated trace operations under measurement. */
#include "kymo_trace.h"
#include "benchmark.h"
#include <KymoCore.h>
#include <xtensa/core-macros.h>
static KymoTraceRecorder recorder;
static uint8_t traceFrame[KYMO_TRACE_FRAME_MAX];
/*******************************************************************************
 * traceTicks
 ******************************************************************************/
static uint64_t traceTicks(void *user)
{
    (void)user;
    return static_cast<uint64_t>(XTHAL_GET_CCOUNT());
}

/*******************************************************************************
 * benchTrace
 ******************************************************************************/
uint32_t benchTrace(void)
{
    KymoTraceEvent event;
    uint32_t cycles = 0u;
    for (uint32_t run = 0u; run < RUNS; ++run) {
        const uint32_t start = XTHAL_GET_CCOUNT();
        KymoTrace_Enter(&recorder, 1u, 1u);
        KymoTrace_Exit(&recorder, 1u, 1u);
        cycles += XTHAL_GET_CCOUNT() - start;
        while (KymoTrace_Pop(&recorder, &event) == KYMO_OK) {
            sink += event.depth;
        }
    }
    return cycles;
}

/*******************************************************************************
 * benchCrc16
 ******************************************************************************/
uint32_t benchCrc16(void)
{
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        traceFrame[0] = static_cast<uint8_t>(run);
        sink += KymoTrace_Crc16(traceFrame, sizeof(traceFrame));
    }
    return XTHAL_GET_CCOUNT() - start;
}
/*******************************************************************************
 * benchmarkTraceInit
 ******************************************************************************/
void benchmarkTraceInit(void)
{
    KymoTraceConfig config = {};
    config.clock_ticks = traceTicks;
    config.session_id = 1u;
    config.clock_domain_id = 1u;
    (void)KymoTrace_Init(&recorder, &config);
    for (size_t index = 0u; index < sizeof(traceFrame); ++index) {
        traceFrame[index] = static_cast<uint8_t>(index * 7u);
    }
}
