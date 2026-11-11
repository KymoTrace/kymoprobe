/* SPDX-License-Identifier: MIT */
/** @file kymo_memory.cpp @brief Isolated memory operations under measurement. */
#include "kymo_memory.h"
#include "benchmark.h"
#include <KymoCore.h>
#include <xtensa/core-macros.h>
static KymoMemory memory;
static const KymoMemoryEntry entries[] = {{"bench", 4096u, KYMO_MEMORY_STACK_PEAK, 200u}};
/*******************************************************************************
 * readMemory
 ******************************************************************************/
static uint8_t readMemory(void *context, size_t index, uint32_t *bytes)
{
    (void)context;
    (void)index;
    *bytes = 1234u;
    return 1u;
}

/*******************************************************************************
 * benchMemory
 ******************************************************************************/
uint32_t benchMemory(void)
{
    KymoDataPoint point = {};
    uint32_t nowMs = 0u;
    const uint32_t start = XTHAL_GET_CCOUNT();
    for (uint32_t run = 0u; run < RUNS; ++run) {
        nowMs += 1u;
        if (KymoMemory_NextSample(&memory, nowMs, &point) == KYMO_OK) {
            sink += point.id;
        }
    }
    return XTHAL_GET_CCOUNT() - start;
}
/*******************************************************************************
 * benchmarkMemoryInit
 ******************************************************************************/
void benchmarkMemoryInit(void)
{
    const KymoMemoryConfig config = {entries, KYMO_COUNT_OF(entries), 1u, 0u, readMemory, nullptr};
    (void)KymoMemory_Init(&memory, &config);
}
