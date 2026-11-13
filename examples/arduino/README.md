# Arduino

Start with [quickstart](quickstart/README.md) to plot two ADC variables using
KymoCore 7.4.0's `KymoPlotBinding` and `Kymo_SampleBound`. The example uses
`Kymo_Init` and `Kymo_Main` with default settings. Its README explains how to
enable device timestamps.

Use `simple_analog_example` for two generated waveform channels or
`multi_sensor_example` for Y/XY/XYZ. These examples use Endpoint and the
shared waveform configuration in `lib/ProbeEndpoint/src/probe/kymo_plot.cpp`.
Their PlatformIO configurations enable the required Endpoint and payload
features.

Select Uno, Nano or Mega in each project's `platformio.ini`. All examples
consume the official KymoCore release pinned in `lib/KymoCore`.
Serial connection is 115200 baud, 8N1, KymoStudio binary v6.
See [example build instructions](../README.md).
