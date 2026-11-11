/* SPDX-License-Identifier: MIT */
/** @file kymo_port.c @brief HAL UART adapter; borrows TX until its interrupt completes. */
#include "board.h"
/*******************************************************************************
 * clock_ms
 ******************************************************************************/
static uint32_t clock_ms(void *user) { (void)user; return HAL_GetTick(); }
/*******************************************************************************
 * uart_busy
 ******************************************************************************/
static uint8_t uart_busy(void *user) {
    return ((UART_HandleTypeDef *)user)->gState != HAL_UART_STATE_READY;
}
/*******************************************************************************
 * uart_write
 ******************************************************************************/
static size_t uart_write(void *user, const uint8_t *bytes, size_t length) {
    // HAL borrows the runtime's persistent buffer until the TX interrupt ends.
    return HAL_UART_Transmit_IT((UART_HandleTypeDef *)user, (uint8_t *)bytes,
                                (uint16_t)length) == HAL_OK ? length : 0;
}
/*******************************************************************************
 * kymoPortConfig
 ******************************************************************************/
KymoEndpointConfig kymoPortConfig(void)
{
    KymoEndpointConfig config = {0};
    config.clock_ms = clock_ms;
    config.tx.write = uart_write;
    config.tx.busy = uart_busy;
    config.tx.transport_user = &huart2;
    return config;
}
