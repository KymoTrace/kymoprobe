/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file network_demo.cpp
 * @brief Selectable ESP32 TCP, MQTT or combined waveform application.
 * @details Each enabled transport owns a separate fixed runtime buffer.
 * @warning Arduino WiFi and PubSubClient use SDK heap and synchronous calls.
 * Connections, DNS and writes can block; this adapter is not hard real time.
 */
#include "demo_selection.h"
#include <KymoCore.h>
#if KYMO_DEMO_NETWORK_ENABLE && KYMO_EFFECTIVE_RUNTIME
#include <Arduino.h>
#include <WiFi.h>
#include <probe/kymo_init.h>
#include "features/network/kymo_plot.h"
#include <new>
#include "network_secrets.h"
#if KYMO_DEMO_MQTT_ENABLE
#include <PubSubClient.h>
static const char MQTT_HOST[] = "broker.hivemq.com";
static const char MQTT_TOPIC[] = "kymotrace/demo/7c92e4a83f16/saw";
static const char MQTT_CLIENT_ID[] = "kymo-7c92e4a83f16-board";
static WiFiClient mqttSocket;
static PubSubClient mqtt(mqttSocket);
static ProbeCore mqttContext;
static KymoEndpointPlot mqttPlot;
static uint32_t lastMqttAttemptMs;
#endif
#if KYMO_DEMO_TCP_ENABLE
static const uint16_t TCP_PORT = 2323u;
static WiFiServer tcpServer(TCP_PORT);
static WiFiClient tcpClient;
static ProbeCore tcpContext;
static KymoEndpointPlot tcpPlot;
#endif
static uint32_t lastStatusMs;
static bool wasWifiConnected = false;
static bool isReady = false;

/** @brief Read monotonic milliseconds. @param[in] user Unused.
 * @return Milliseconds modulo 2^32. */
/*******************************************************************************
 * readClockMs
 ******************************************************************************/
static uint32_t readClockMs(void *user)
{
    (void)user;
    return millis();
}

/** @brief Generate a waveform. @param[in] user Unused.
 * @param[in] id 2 selects sine, 3 sawtooth. @param[out] out Sample destination.
 * @return One when a sample was written, otherwise zero. */

#if KYMO_DEMO_TCP_ENABLE
/** @brief Copy a TCP prefix. @param[in] user Unused.
 * @param[in] bytes Borrowed bytes. @param[in] length Available bytes.
 * @return Bytes consumed, or zero when disconnected. */
/*******************************************************************************
 * writeTcp
 ******************************************************************************/
static size_t writeTcp(void *user, const uint8_t *bytes, size_t length)
{
    size_t written = 0u;
    (void)user;
    if ((bytes != nullptr) && tcpClient.connected()) {
        written = tcpClient.write(bytes, length);
    }
    return written;
}
/*******************************************************************************
 * makeTcpConfig
 ******************************************************************************/
static KymoEndpointConfig makeTcpConfig()
{
    KymoEndpointConfig config = {};
    config.clock_ms = readClockMs;
    config.tx.write = writeTcp;
    return config;
}
static const KymoEndpointConfig tcpConfig = makeTcpConfig();
/*******************************************************************************
 * initializeTcpStream
 ******************************************************************************/
static KymoStatus initializeTcpStream(void)
{
    const KymoEndpointPlotConfig plot = networkPlotConfig(true);
    KymoStatus status = KYMO_BAD_CONFIG;
    tcpContext.~ProbeCore();
    new (&tcpContext) ProbeCore{};
    tcpPlot.~KymoEndpointPlot();
    new (&tcpPlot) KymoEndpointPlot{};
    status = probeCoreInit(&tcpContext, &tcpConfig);
    if (status == KYMO_OK) { status = KymoEndpointPlot_Init(&tcpContext.endpoint, &tcpPlot, &plot); }
    return status;
}

/** @brief Accept one client and reset new streams to a complete frame. */
/*******************************************************************************
 * serviceTcp
 ******************************************************************************/
static void serviceTcp(void)
{
    WiFiClient extraClient;
    if (tcpServer.hasClient()) {
        if (!tcpClient.connected()) {
            tcpClient.stop();
            tcpClient = tcpServer.available();
            (void)tcpClient.setNoDelay(true);
            (void)tcpClient.setTimeout(1u);
            (void)initializeTcpStream();
            (void)Serial.println("KYMO_TCP_CONNECTED=1");
        } else {
            extraClient = tcpServer.available();
            extraClient.stop();
        }
    }
}
#endif

#if KYMO_DEMO_MQTT_ENABLE
/** @brief Publish one complete frame. @param[in] user Unused.
 * @param[in] bytes Borrowed frame. @param[in] length Frame length.
 * @return Whole length on success, zero for retry. */
/*******************************************************************************
 * writeMqtt
 ******************************************************************************/
static size_t writeMqtt(void *user, const uint8_t *bytes, size_t length)
{
    size_t written = 0u;
    (void)user;
    if ((bytes != nullptr) && mqtt.connected() &&
        mqtt.publish(MQTT_TOPIC, bytes, length, false)) {
        written = length;
    }
    return written;
}
/*******************************************************************************
 * makeMqttConfig
 ******************************************************************************/
static KymoEndpointConfig makeMqttConfig()
{
    KymoEndpointConfig config = {};
    config.clock_ms = readClockMs;
    config.tx.write = writeMqtt;
    return config;
}
static const KymoEndpointConfig mqttConfig = makeMqttConfig();
/*******************************************************************************
 * initializeMqttStream
 ******************************************************************************/
static KymoStatus initializeMqttStream(void)
{
    const KymoEndpointPlotConfig plot = networkPlotConfig(false);
    KymoStatus status = KYMO_BAD_CONFIG;
    mqttContext.~ProbeCore();
    new (&mqttContext) ProbeCore{};
    mqttPlot.~KymoEndpointPlot();
    new (&mqttPlot) KymoEndpointPlot{};
    status = probeCoreInit(&mqttContext, &mqttConfig);
    if (status == KYMO_OK) { status = KymoEndpointPlot_Init(&mqttContext.endpoint, &mqttPlot, &plot); }
    return status;
}

/** @brief Retry MQTT every five seconds and service its connection. */
/*******************************************************************************
 * serviceMqtt
 ******************************************************************************/
static void serviceMqtt(void)
{
    const uint32_t nowMs = millis();
    if (!mqtt.connected() && (nowMs - lastMqttAttemptMs >= 5000u)) {
        lastMqttAttemptMs = nowMs;
        if (mqtt.connect(MQTT_CLIENT_ID)) {
            (void)initializeMqttStream();
        }
    }
    (void)mqtt.loop();
}
#endif

/** @brief Print USB connection status without disclosing credentials. */
/*******************************************************************************
 * printStatus
 ******************************************************************************/
static void printStatus(void)
{
    (void)Serial.print("KYMO_WIFI_STATUS=");
    (void)Serial.println(static_cast<int32_t>(WiFi.status()));
    if (WiFi.status() == WL_CONNECTED) {
        (void)Serial.print("KYMO_WIFI_IP=");
        (void)Serial.println(WiFi.localIP());
    }
#if KYMO_DEMO_MQTT_ENABLE
    (void)Serial.print("KYMO_MQTT_CONNECTED=");
    (void)Serial.println(mqtt.connected() ? 1 : 0);
    (void)Serial.print("KYMO_MQTT_STATE=");
    (void)Serial.println(mqtt.state());
#endif
}

/** @brief Advance WiFi and selected transport connections. */
/*******************************************************************************
 * serviceNetwork
 ******************************************************************************/
static void serviceNetwork(void)
{
    const bool isWifiConnected = WiFi.status() == WL_CONNECTED;
    const uint32_t nowMs = millis();
    if (isWifiConnected != wasWifiConnected) {
#if KYMO_DEMO_TCP_ENABLE
        tcpClient.stop();
        tcpServer.end();
        if (isWifiConnected) {
            tcpServer.begin();
        }
#endif
#if KYMO_DEMO_MQTT_ENABLE
        mqttSocket.stop();
        lastMqttAttemptMs = nowMs - 5000u;
#endif
        wasWifiConnected = isWifiConnected;
        printStatus();
    }
    if (isWifiConnected) {
#if KYMO_DEMO_TCP_ENABLE
        serviceTcp();
#endif
#if KYMO_DEMO_MQTT_ENABLE
        serviceMqtt();
#endif
    }
    if (nowMs - lastStatusMs >= 5000u) {
        lastStatusMs = nowMs;
        printStatus();
    }
}

/** @brief Initialize WiFi, selected transports and fixed runtimes. */
/*******************************************************************************
 * demoNetworkInit
 ******************************************************************************/
void demoNetworkInit(void)
{
    Serial.begin(115200u);
    WiFi.persistent(false);
    (void)WiFi.mode(WIFI_STA);
    (void)WiFi.setAutoReconnect(true);
    (void)WiFi.begin(KYMO_WIFI_SSID, KYMO_WIFI_PASSWORD);
    isReady = true;
#if KYMO_DEMO_TCP_ENABLE
    isReady = (initializeTcpStream() == KYMO_OK) && isReady;
#endif
#if KYMO_DEMO_MQTT_ENABLE
    (void)mqttSocket.setTimeout(1u);
    (void)mqtt.setServer(MQTT_HOST, 1883u);
    (void)mqtt.setSocketTimeout(1u);
    isReady = (initializeMqttStream() == KYMO_OK) && isReady;
#endif
    (void)Serial.println(isReady ? "KYMO_NETWORK_READY=1" : "KYMO_NETWORK_READY=0");
}

/** @brief Sample connected channels and yield to the SDK. */
/*******************************************************************************
 * demoNetworkMain
 ******************************************************************************/
void demoNetworkMain(void)
{
    serviceNetwork();
    if (isReady && wasWifiConnected) {
#if KYMO_DEMO_TCP_ENABLE
        if (tcpClient.connected()) {
            (void)KymoEndpoint_Main(&tcpContext.endpoint);
        }
#endif
#if KYMO_DEMO_MQTT_ENABLE
        if (mqtt.connected()) {
            (void)KymoEndpoint_Main(&mqttContext.endpoint);
        }
#endif
    }
    yield();
}
#endif
