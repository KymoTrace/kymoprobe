# ESP32: CPU-Kosten von KymoCore messen

Misst mit dem Zyklenzähler des ESP32 (CCOUNT), wie viel Rechenzeit die einzelnen
KymoCore-Operationen kosten. Alle 5 s läuft jede Operation 1000-mal; ausgegeben wird
der Mittelwert in Takten und Mikrosekunden als Textzeile über die serielle
Schnittstelle (115200 Baud, `pio device monitor`).

| Zeile | Was gemessen wird |
|---|---|
| `DATA frame encode+CRC8` | Ein Messwert-Frame mit Zeitstempel und CRC-8 |
| `trace ENTER+EXIT` | Ein Paar aus Eintritt und Austritt im Trace-Rekorder |
| `trace CRC16 240 B` | CRC-16 über einen vollen Trace-Frame |
| `memory sample` | Ein Messpunkt von `KymoMemory` |

Die beiden Umgebungen unterscheiden sich nur in `KYMO_ENABLE_CRC_TABLE`:

```bash
pio run -e esp32-benchmark-bitwise -t upload
```

```bash
pio run -e esp32-benchmark-table -t upload
```

Die Tabellen kosten auf Cortex-M4 244 B (CRC-8) und 486 B (CRC-16) Flash
(arm-none-eabi-gcc 12.3, `-Os`); auf dem ESP32 ähnlich. Interrupts bleiben an,
einzelne Ausreißer glättet der Mittelwert.

Die gemessenen Werte gehören danach in den KymoCore-GUIDE und auf die Website.
Das Beispiel verwendet das offizielle KymoCore-Release 7.3.0 aus dem gepinnten
Submodul `lib/KymoCore`. Ein benachbarter Entwicklungscheckout wird nicht benötigt.
