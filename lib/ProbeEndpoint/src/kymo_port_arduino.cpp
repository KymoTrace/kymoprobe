/* SPDX-License-Identifier: MIT */
/** @file kymo_port_arduino.cpp @brief Bounded copying Arduino Serial port. */
#include "probe/kymo_port.h"
#if KYMO_EFFECTIVE_ENDPOINT && defined(ARDUINO)
#include <Arduino.h>
/*******************************************************************************
 * probeSerialClock
 ******************************************************************************/
uint32_t probeSerialClock(void *user)
{
    (void)user;
    return millis();
}
/*******************************************************************************
 * serialWrite
 ******************************************************************************/
static size_t serialWrite(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    const int32_t available = Serial.availableForWrite();
    (void)user;
    if ((bytes != nullptr) && (available > 0)) {
        if (length > static_cast<size_t>(available)) { length = static_cast<size_t>(available); }
        accepted = Serial.write(bytes, length);
    }
    return accepted;
}
#if KYMO_EFFECTIVE_PARAMETERS || KYMO_EFFECTIVE_WATCH
/*******************************************************************************
 * serialRead
 ******************************************************************************/
static size_t serialRead(void *user, uint8_t *bytes, size_t capacity)
{
    size_t count = 0u;
    (void)user;
    if (bytes != nullptr) {
        while ((count < capacity) && (Serial.available() > 0)) {
            const int32_t value = Serial.read();
            if (value < 0) { break; }
            bytes[count++] = static_cast<uint8_t>(value);
        }
    }
    return count;
}
#endif
/*******************************************************************************
 * probeSerialConfig
 ******************************************************************************/
KymoEndpointConfig probeSerialConfig(void)
{
    KymoEndpointConfig config = {};
    config.clock_ms = probeSerialClock;
    config.tx.write = serialWrite;
#if KYMO_EFFECTIVE_PARAMETERS || KYMO_EFFECTIVE_WATCH
    config.read = serialRead;
#endif
    return config;
}
#endif
