/* SPDX-License-Identifier: MIT */
/** @file kymo_port.cpp @brief WiFi/MQTT application port. */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "kymo_port.h"
// Edit credentials, endpoint and channels here. Do not commit real secrets.
static const char *WIFI_SSID = "YourWiFiSSID";
static const char *WIFI_PASSWORD = "YourWiFiPassword";
static const char *MQTT_BROKER = "192.168.1.100";
static const char *MQTT_TOPIC = "sensor/data";
static WiFiClient wifi;
static PubSubClient mqtt(wifi);
static uint32_t last_connect;
/*******************************************************************************
 * clock_ms
 ******************************************************************************/
static uint32_t clock_ms(void *) { return millis(); }
/*******************************************************************************
 * service
 ******************************************************************************/
static void service(void *) {
    uint32_t now = millis();
    // PubSubClient connect() is synchronous: not a hard real-time transport.
    if (WiFi.status() == WL_CONNECTED && !mqtt.connected() &&
        (uint32_t)(now - last_connect) >= 5000u) {
        last_connect = now;
        mqtt.connect("KymoECU"); // choose a unique ID for each device
    }
    mqtt.loop();
}
/*******************************************************************************
 * busy
 ******************************************************************************/
static uint8_t busy(void *) { return !mqtt.connected(); }
/*******************************************************************************
 * write_frame
 ******************************************************************************/
static size_t write_frame(void *, const uint8_t *bytes, size_t length) {
    // Message transport: accept the entire binary payload or retry later.
    return mqtt.publish(MQTT_TOPIC, bytes, length) ? length : 0;
}

/*******************************************************************************
 * kymoMqttPortInit
 ******************************************************************************/
void kymoMqttPortInit(void)
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    mqtt.setServer(MQTT_BROKER, 1883);
    mqtt.setSocketTimeout(1);
    last_connect = millis() - 5000u;
}
/*******************************************************************************
 * kymoMqttPortConfig
 ******************************************************************************/
KymoEndpointConfig kymoMqttPortConfig(void)
{
    KymoEndpointConfig config = {};
    config.clock_ms = clock_ms;
    config.tx.write = write_frame;
    config.tx.busy = busy;
    config.service = service;
    return config;
}
