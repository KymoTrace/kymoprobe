/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h @brief Copying MQTT transport; synchronous SDK connection calls.
 * @par Usage Initialize hardware, then provide the port config to base Init. */
#pragma once
#include <endpoint/kymo_endpoint.h>
/** @brief Start WiFi and set the broker. */
void kymoMqttPortInit(void);
/** @brief Construct MQTT callbacks. @return Port configuration without buffers. */
KymoEndpointConfig kymoMqttPortConfig(void);
