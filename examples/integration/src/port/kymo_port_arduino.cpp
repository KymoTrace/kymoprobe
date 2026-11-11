/* SPDX-License-Identifier: MIT */
/** @file kymo_port_arduino.cpp
 * @brief Arduino Serial adapter for ESP32 and Uno; called by one superloop.
 * @details SDK dependencies and Serial ownership stay in this file.
 */
#include "kymo_port.h"
#include <Arduino.h>

static const uint32_t SERIAL_BAUD_RATE = 115200u;

/*******************************************************************************
 * kymoPortInit
 ******************************************************************************/
void kymoPortInit(void)
{
    Serial.begin(SERIAL_BAUD_RATE);
}
/*******************************************************************************
 * kymoPortClockMs
 ******************************************************************************/
uint32_t kymoPortClockMs(void *user)
{
    uint32_t nowMs = static_cast<uint32_t>(millis());
    (void)user;
    return nowMs;
}
/*******************************************************************************
 * kymoPortClockTicks
 ******************************************************************************/
uint64_t kymoPortClockTicks(void *user)
{
    static uint32_t previousMs = 0u;
    static uint64_t ticks = 0u;
    uint32_t nowMs = kymoPortClockMs(user);
    ticks += static_cast<uint32_t>(nowMs - previousMs);
    previousMs = nowMs;
    return ticks;
}
/*******************************************************************************
 * kymoPortRead
 ******************************************************************************/
size_t kymoPortRead(void *user, uint8_t *bytes, size_t capacity)
{
    size_t count = 0u;
    (void)user;
    if (bytes != nullptr) {
        while ((count < capacity) && (Serial.available() > 0)) {
            int32_t value = static_cast<int32_t>(Serial.read());
            if (value < 0) {
                break;
            }
            bytes[count] = static_cast<uint8_t>(value);
            ++count;
        }
    }
    return count;
}
/*******************************************************************************
 * kymoPortWrite
 ******************************************************************************/
size_t kymoPortWrite(void *user, const uint8_t *bytes, size_t length)
{
    size_t accepted = 0u;
    int32_t available = static_cast<int32_t>(Serial.availableForWrite());
    size_t count = 0u;
    (void)user;
    if ((bytes != nullptr) && (available > 0)) {
        count = static_cast<size_t>(available);
        count = length < count ? length : count;
        accepted = Serial.write(bytes, count);
    }
    return accepted;
}
