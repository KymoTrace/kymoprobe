/* SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Christof Seidel */
#include <Arduino.h>
#include <probe/kymo_init.h>
#include <probe/kymo_port.h>
#include <probe/kymo_plot.h>

#if KYMO_EFFECTIVE_RUNTIME
static ProbeCore core;
static KymoStatus status = KYMO_BAD_CONFIG;

/*******************************************************************************
 * setup
 ******************************************************************************/
void setup() {
    Serial.begin(115200); // CDC line coding; USB itself is not limited to this baud.
    const KymoEndpointConfig port = probeSerialConfig();
    const KymoEndpointPlotConfig plot = probeWaveformConfig();
    status = probeCoreInit(&core, &port);
    if (status == KYMO_OK) { status = probeWaveformInit(&core.endpoint, &plot); }
}

/*******************************************************************************
 * loop
 ******************************************************************************/
void loop() {
    if ((status == KYMO_OK) || (status == KYMO_IDLE) || (status == KYMO_BUSY) || (status == KYMO_BAD_SAMPLE)) {
        status = KymoEndpoint_Main(&core.endpoint);
    }
}
#else
/*******************************************************************************
 * setup
 ******************************************************************************/
void setup() {}

/*******************************************************************************
 * loop
 ******************************************************************************/
void loop() {}
#endif
