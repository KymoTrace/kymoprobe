/* SPDX-License-Identifier: MIT */
/** @file kymo_port.cpp @brief JSON formatting and Arduino Serial output. */
#include <Arduino.h>
#include <stdio.h>
#include "kymo_port.h"
/*******************************************************************************
 * textPortSend
 ******************************************************************************/
void textPortSend(uint32_t id, float value)
{
    char line[48] = {};
    const int length = snprintf(line, sizeof(line), "{\"id\":%lu,\"value\":%.5f}\n",
                                static_cast<unsigned long>(id), static_cast<double>(value));
    if ((length > 0) && (static_cast<size_t>(length) < sizeof(line))) {
        (void)Serial.write(reinterpret_cast<const uint8_t *>(line), static_cast<size_t>(length));
    }
}
