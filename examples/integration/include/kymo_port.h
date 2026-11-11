/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h
 * @brief Project-owned platform adapter; no hardware SDK in this header.
 * @details The Arduino implementation copies bytes into the serial TX queue.
 * @par Usage Supply clock/write callbacks to the common base configuration.
 */
#ifndef EXAMPLE_KYMO_PORT_H
#define EXAMPLE_KYMO_PORT_H
#include <stddef.h>
#include <stdint.h>
/** @brief Start the hardware port at the configured baud rate.
 * @par Usage Once before KymoApp_Init. */
void kymoPortInit(void);
/** @brief Return the wrapping millisecond clock.
 * @param[in] user Unused; NULL allowed.
 * @return Time in milliseconds, modulo 2^32. @par Usage Endpoint clock callback. */
uint32_t kymoPortClockMs(void *user);
/** @brief Extended monotonic clock, 1000 ticks/second, for Trace metadata.
 * @param[in] user Unused; NULL allowed.
 * @return Millisecond ticks extended beyond the 32-bit wrap.
 * @par Usage Serialized calls at least once per millisecond-clock wrap. */
uint64_t kymoPortClockTicks(void *user);
/** @brief Read a bounded prefix of incoming control bytes without waiting.
 * @param[in] user Unused; NULL allowed.
 * @param[out] bytes Destination; NULL reads nothing.
 * @param[in] capacity Maximum bytes to read.
 * @return Copied byte count, 0..capacity. @par Usage Endpoint RX callback. */
size_t kymoPortRead(void *user, uint8_t *bytes, size_t capacity);
/** @brief Copy at most the currently available number of bytes to Serial.
 * @details Serial is reserved for KymoCore; no concurrent writers. No borrowed buffer.
 * @param[in] user Unused; NULL allowed.
 * @param[in] bytes Encoded binary frame; NULL accepts nothing.
 * @param[in] length Offered byte count.
 * @return Accepted prefix length, 0..length. Zero is ordinary backpressure.
 * @par Usage Endpoint write callback; never mix Serial.print with this stream. */
size_t kymoPortWrite(void *user, const uint8_t *bytes, size_t length);
#endif
