# KymoProbe examples

All examples use the official KymoCore release pinned in `lib/KymoCore`
(currently **7.3.3**). Probe release tags use the same version as Core;
see [release and tag policy](../docs/releases.md).

For the modular Endpoint integration start with
[integration/README.md](integration/README.md) and
[integration/src/main.cpp](integration/src/main.cpp): one base and separate
Plot, Parameter, Watch, Memory and Trace headers/implementations. The explicit
feature configs and status checks remain visible. The existing application,
transport and generated demos also use Endpoint and separate feature files.
The text-protocol diagnostic and low-level benchmark retain their diagnostic
purpose with separate feature and transport/reporting code.

The larger transport, maximum-throughput and feature applications are selected
by the repository's central `src/main.cpp`. See [demo modes](../docs/demo-modes.md).
The standalone board examples below remain available in KymoProbe. KymoCore
contains only a few small portable/Arduino reference examples.

Primary examples (run from repository root):

```sh
pio run -e nodemcu-32s
pio run -d examples/integration -e esp32dev -e uno
pio run -d examples/arduino/simple_analog_example -e uno
pio run -d examples/arduino/multi_sensor_example -e uno
pio run -d examples/esp32/serial_example -e esp32dev
pio run -d examples/esp32/wifi_mqtt_example -e esp32dev
pio run -d examples/esp32/trace_uart -e esp32-trace
pio run -d examples/esp32/measurement_signals
pio run -d examples/esp32/task_trace_example -e esp32-runtime-test
```

For numeric live-analysis tests use [ESP32 measurement signals](esp32/measurement_signals/README.md):
two timestamped sine/sawtooth channels with known FFT frequencies and measurement
values. The previous running task/ISR/pipeline firmware is preserved separately as
[task trace example](esp32/task_trace_example/RESTORE.md), including its active
generated configuration and source checksums.

Arduino and serial ESP32 examples share `lib/ProbeEndpoint` bindings and
send deterministic waveforms through Endpoint. Modify the sample
callback to acquire actual sensor values. The multidimensional example uses
three independently scheduled channels. The `.ino` is the single entrypoint
source; PlatformIO's `src/main.cpp` includes it to avoid duplicate code.
Install both KymoCore and the application support library `lib/ProbeEndpoint`
when using these sketches in the Arduino IDE.

KymoCore defaults to scalar Y plus the C runtime. These example PlatformIO
files explicitly enable timestamps; the multidimensional example additionally
enables X and Z. The repository-root application uses the minimal defaults.
For Arduino IDE/other build systems, pass matching `KYMO_ENABLE_*` compiler
definitions or `KYMO_USER_CONFIG_HEADER` to the sketch and library; a sketch-local
define is not sufficient. Keep the pinned library checkout unchanged.
C++ push users must additionally enable `KYMO_ENABLE_CPP=1`.
See the feature table in `../lib/KymoCore/README.md`.

Serial examples: 115200 baud, 8N1, binary v6. Connect KymoStudio to the board's
serial port. Do not share that port with a text monitor. MQTT: edit WiFi/broker
settings, subscribe KymoStudio to `sensor/data`; one whole frame per payload.
PubSubClient connect is synchronous even though sampling is cyclic.

The separate `esp32/trace_uart` project instruments explicit scope and marker
sites, then multiplexes canonical session metadata, its generated symbol
dictionary and trace batches over UART v2. In KymoStudio create a Runtime trace
chart for the timeline or select a span and open its Function flow. Its serial
speed is 460800 baud; close other serial monitors first. Scheduler and ISR
coverage remain disabled until a complete target adapter is verified.

Optional portability examples:

```sh
pio run -d examples/stm32/uart_example -e nucleo_f401re
pio run -d examples/stm32/usb_cdc_example -e bluepill_f103c8
pio run -d examples/native -e native
```

STM32 UART is C with STM32Cube HAL and interrupt-driven USART2 TX on PA2;
no CubeMX-generated files are required. Use a 3.3 V serial adapter or the
Nucleo ST-Link virtual COM port. USB CDC uses STM32duino on the Blue Pill's
native USB connector (not the programmer connector). Other listed boards
must be validated on their actual hardware. Native requires gcc/g++ on PATH;
run the generated `.pio/build/native/program` (Windows: `program.exe`).

Only the ESP32 and Arduino examples are required for the primary workflow.
See `../docs/verification.md` for exactly which targets were compiled here;
compilation is not a hardware test.
