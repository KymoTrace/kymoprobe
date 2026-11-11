/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
/** @file stress_demo.cpp
 * @brief Configurable ESP32 load generator using all 256 protocol channels.
 * @details 32-bit Arduino ESP32 superloop; integer waveform generation also
 * supports ESP32 variants without an FPU. The application owns the output batch
 * and copies borrowed core bytes before returning. No application heap usage.
 * @warning The Arduino Serial/WiFi adapters allocate internally; TCP writes
 * may block under backpressure. UDP has none: undeliverable datagrams are
 * dropped on the board, unseen by KymoStudio's counters (check ramp gaps).
 * This SDK adapter is not a hard real-time driver.
 */
#include <KymoCore.h>
#include "demo_selection.h"
#if KYMO_CONFIG_API_VERSION != 2
#error "KymoProbe stress demo requires KymoCore configuration API version 2"
#endif
#if defined(KYMO_STRESS_ENABLE) && KYMO_STRESS_ENABLE && KYMO_EFFECTIVE_RUNTIME
#include <Arduino.h>
#include <string.h>

#include "features/stress/Config.h"
#include "features/stress/kymo_plot.h"
#include <probe/kymo_init.h>
#include <new>
#if KYMO_STRESS_WIFI
#include <WiFi.h>
#include "network_secrets.h"
static bool s_wasWifiConnected = false;
#endif
#if KYMO_STRESS_TCP_ENABLE
static WiFiServer s_tcpServer(2323u);
static WiFiClient s_tcpClient;
#endif
#if KYMO_STRESS_UDP_ENABLE
#include <WiFiUdp.h>
#ifndef KYMO_STRESS_UDP_HOST
#error "Define KYMO_STRESS_UDP_HOST (the KymoStudio PC's IPv4 address) in network_secrets.h"
#endif
static WiFiUDP s_udp;
/* One datagram holds whole frames only: Studio decodes each datagram on its own. */
static const uint16_t OUTPUT_CAPACITY = 1400u;
#else
static const uint16_t OUTPUT_CAPACITY = 4096u;
#endif
static const uint16_t MAIN_CALLS_PER_LOOP = 512u;
static ProbeCore s_context;
static KymoEndpointPlot s_plot;
static uint8_t s_output[OUTPUT_CAPACITY];
static uint16_t s_outputLength = 0u;
static uint16_t s_outputOffset = 0u;
static bool s_isReady = false;

/** @brief Read monotonic milliseconds. @param[in] user Unused.
 * @return SDK tick modulo 2^32. */
/*******************************************************************************
 * readStressClockMs
 ******************************************************************************/
static uint32_t readStressClockMs(void *user)
{
    (void)user;
    return millis();
}

/** @brief Copy one core frame into application-owned batch storage.
 * @param[in] user Unused. @param[in] bytes Borrowed frame bytes.
 * @param[in] length Byte count. @return Length when copied, zero if full. */
/*******************************************************************************
 * enqueueStressFrame
 ******************************************************************************/
static size_t enqueueStressFrame(void *user, const uint8_t *bytes, size_t length)
{
    size_t written = 0u;
    (void)user;
    if ((bytes != nullptr) && (length <= static_cast<size_t>(OUTPUT_CAPACITY - s_outputLength))) {
        (void)memcpy(s_output + s_outputLength, bytes, length);
        s_outputLength = static_cast<uint16_t>(s_outputLength + length);
        written = length;
    }
    return written;
}

/** @brief Apply backpressure before sampling when a full frame cannot fit.
 * @param[in] user Unused. @return One when the batch has insufficient space. */



/** @brief Restart at a frame boundary, discarding stale data after reconnect. */
/*******************************************************************************
 * resetStressStream
 ******************************************************************************/
static void resetStressStream()
{
    s_outputLength = 0u;
    s_outputOffset = 0u;
    // Copying transport: old socket is closed, no driver borrows Endpoint bytes.
    s_context.~ProbeCore();
    new (&s_context) ProbeCore{};
    s_plot.~KymoEndpointPlot();
    new (&s_plot) KymoEndpointPlot{};
    KymoEndpointConfig port = {};
    const KymoEndpointPlotConfig plot = stressPlotConfig();
    port.clock_ms = readStressClockMs;
    port.tx.write = enqueueStressFrame;
    s_isReady = probeCoreInit(&s_context, &port) == KYMO_OK;
    if (s_isReady) { s_isReady = KymoEndpointPlot_Init(&s_context.endpoint, &s_plot, &plot) == KYMO_OK; }
}

/** @brief Consume a batch prefix once, preserving unsent bytes for the next loop.
 * @note Not ISR-safe. UART writes never exceed available TX space. */
/*******************************************************************************
 * flushStressOutput
 ******************************************************************************/
static void flushStressOutput()
{
    uint16_t remaining = static_cast<uint16_t>(s_outputLength - s_outputOffset);
    uint16_t written = 0u;
#if !KYMO_STRESS_WIFI
    int32_t available = static_cast<int32_t>(Serial.availableForWrite());
#endif
    if (remaining > 0u) {
#if KYMO_STRESS_TCP_ENABLE
        if (s_tcpClient.connected()) {
            written = static_cast<uint16_t>(s_tcpClient.write(s_output + s_outputOffset, remaining));
        }
#elif KYMO_STRESS_UDP_ENABLE
        /* No backpressure by design: a datagram the stack cannot take is dropped,
         * so overload shows up in Studio instead of slowing the sender. */
        if (s_udp.beginPacket(KYMO_STRESS_UDP_HOST, KYMO_STRESS_UDP_PORT) != 0) {
            (void)s_udp.write(s_output + s_outputOffset, remaining);
            (void)s_udp.endPacket();
        }
        written = remaining;
#else
        if (available > 0) {
            if (static_cast<uint32_t>(available) < remaining) {
                remaining = static_cast<uint16_t>(available);
            }
            written = static_cast<uint16_t>(Serial.write(s_output + s_outputOffset, remaining));
        }
#endif
        s_outputOffset = static_cast<uint16_t>(s_outputOffset + written);
        if (s_outputOffset == s_outputLength) {
            s_outputLength = 0u;
            s_outputOffset = 0u;
        }
    }
}

#if KYMO_STRESS_UDP_ENABLE
/** @brief Restart the stream at a frame boundary after each WiFi (re)connect. */
/*******************************************************************************
 * serviceStressNetwork
 ******************************************************************************/
static void serviceStressNetwork()
{
    const bool isWifiConnected = WiFi.status() == WL_CONNECTED;
    if (isWifiConnected != s_wasWifiConnected) {
        resetStressStream();
        if (isWifiConnected) {
            (void)Serial.print("KYMO_STRESS_IP=");
            (void)Serial.println(WiFi.localIP());
        }
        s_wasWifiConnected = isWifiConnected;
    }
}
#endif

#if KYMO_STRESS_TCP_ENABLE
/** @brief Accept one TCP client; reset the stream after WiFi or TCP reconnect. */
/*******************************************************************************
 * serviceStressNetwork
 ******************************************************************************/
static void serviceStressNetwork()
{
    const bool isWifiConnected = WiFi.status() == WL_CONNECTED;
    WiFiClient extraClient;
    if (isWifiConnected != s_wasWifiConnected) {
        s_tcpClient.stop();
        s_tcpServer.end();
        resetStressStream();
        if (isWifiConnected) {
            s_tcpServer.begin();
            (void)Serial.print("KYMO_STRESS_IP=");
            (void)Serial.println(WiFi.localIP());
        }
        s_wasWifiConnected = isWifiConnected;
    }
    if (isWifiConnected && s_tcpServer.hasClient()) {
        if (!s_tcpClient.connected()) {
            s_tcpClient.stop();
            s_tcpClient = s_tcpServer.available();
            (void)s_tcpClient.setNoDelay(true);
            (void)s_tcpClient.setTimeout(1u);
            resetStressStream();
        } else {
            extraClient = s_tcpServer.available();
            extraClient.stop();
        }
    }
}
#endif

/** @brief Initialize the channel table and selected SDK transport. */
/*******************************************************************************
 * demoStressInit
 ******************************************************************************/
void demoStressInit()
{
#if KYMO_STRESS_WIFI
    Serial.begin(115200u);
    WiFi.persistent(false);
    (void)WiFi.mode(WIFI_STA);
    (void)WiFi.setAutoReconnect(true);
    (void)WiFi.setSleep(false);
    (void)WiFi.begin(KYMO_WIFI_SSID, KYMO_WIFI_PASSWORD);
#if KYMO_STRESS_TCP_ENABLE
    (void)Serial.println("KYMO_STRESS_TCP_PORT=2323");
#else
    (void)Serial.print("KYMO_STRESS_UDP_TARGET=");
    (void)Serial.println(KYMO_STRESS_UDP_HOST);
#endif
#else
    // Allocate the SDK UART TX ring only at startup; no text on the binary port.
    (void)Serial.setTxBufferSize(8192u);
    Serial.begin(KYMO_STRESS_BAUD);
#endif
    resetStressStream();
}

/** @brief Advance a bounded number of round-robin samples and yield to the SDK. */
/*******************************************************************************
 * demoStressMain
 ******************************************************************************/
void demoStressMain()
{
    bool canSend = s_isReady;
    KymoStatus status = KYMO_OK;
#if KYMO_STRESS_TCP_ENABLE
    serviceStressNetwork();
    canSend = s_isReady && s_wasWifiConnected && s_tcpClient.connected();
#elif KYMO_STRESS_UDP_ENABLE
    serviceStressNetwork();
    canSend = s_isReady && s_wasWifiConnected;
#endif
    if (canSend) {
        flushStressOutput();
        for (uint16_t callIndex = 0u;
             (callIndex < MAIN_CALLS_PER_LOOP) && ((status == KYMO_OK) || (status == KYMO_BUSY)); ++callIndex) {
            if (static_cast<size_t>(OUTPUT_CAPACITY - s_outputLength) < KYMO_FRAME_MAX_SIZE) { break; }
            status = KymoEndpoint_Main(&s_context.endpoint);
        }
        flushStressOutput();
    }
    yield();
}
#endif
