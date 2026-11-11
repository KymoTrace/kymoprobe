# Task-/Trace-Beispiel ab KymoCore 7.3.1

Das aktuelle Beispiel verwendet Endpoint und getrennte Dateien für Trace und Port.
Der unveränderte historische Stand mit den Prüfsummen aus `saved_example.json`
bleibt im Git-Tag `v7.3.0` erhalten. Die Prüfsummen beschreiben diesen historischen
Stand, nicht die aktuelle Endpoint-Implementierung.

```sh
pio run -d examples/esp32/task_trace_example -e esp32-runtime-test
pio run -d examples/esp32/task_trace_example -e esp32-runtime-test -t upload --upload-port COM4
```

KymoStudio verwendet 460800 Baud. Metadaten werden auch für spätere Verbindungen
regelmäßig wiederholt. Der Port wird vor einem Hardwaretest tatsächlich ermittelt.
