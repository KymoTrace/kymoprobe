# ESP32: PI-Regler mit Live-Parametern (Rückkanal)

Ein simulierter Heizungsregler, dessen Verstärkungen und Sollwert KymoStudio während
der Messung verstellt. Das Beispiel zeigt den KymoCore-Rückkanal (`KymoParam`) von
der Parametertabelle bis zur Bestätigung in Studio.

| ID | Signal | Rate |
|---|---|---|
| 0 | Temperatur (°C) der simulierten Heizung | 50 Hz |
| 1 | Heizleistung 0…1, Ausgang des PI-Reglers | 50 Hz |
| 2 | Sollwert (°C) | 50 Hz |
| 3 | Längste Bearbeitungszeit des Rückkanals in µs je 20 ms | 50 Hz |
| 100 | `kp` (Rückmeldung) | bei Änderung |
| 101 | `ki` (Rückmeldung) | bei Änderung |
| 102 | `setpoint` (Rückmeldung) | bei Änderung |

## Wie die Parameter in die Firmware kommen

```cpp
static float kp = 0.20f;
static float ki = 0.02f;
static float setpoint = 45.0f;

static const KymoParameter params[] = {
    KYMO_PARAM(100, kp,       0.0f, 10.0f, ""),
    KYMO_PARAM(101, ki,       0.0f,  2.0f, ""),
    KYMO_PARAM(102, setpoint, 20.0f, 90.0f, "°C"),
};
```

`KYMO_PARAM` nimmt den Variablennamen als Parameternamen. Mehr ist nicht nötig. Studio
fragt beim Verbinden nach der Liste und füllt sein Parameter-Panel daraus.

`kymo_param.cpp` konfiguriert und initialisiert die Parameter; `kymo_plot.cpp`
bindet die Messkanäle an. `loop()` berechnet den Regler und ruft den gemeinsamen
Endpoint auf. Dieser verarbeitet Empfang, Beschreibungen, Rückmeldungen und Versand.
Ein angenommener Wert wirkt ab dem nächsten Regelschritt.

Die Firmware hat das letzte Wort: Ein Sollwert von 95 °C liegt außerhalb von 20…90.
Das Gerät behält dann den alten Wert und meldet ihn zurück, Studio zeigt „abgelehnt“.

## Bauen und ausprobieren

Das Beispiel verwendet KymoCore 7.3.4; das Beispiel baut gegen das Submodul `lib/KymoCore`.

```bash
pio run -d examples/esp32/pi_controller -t upload --upload-port COM4
```

`kymostudio.json` enthält die Quelle (COM4, 115200 Baud) und zwei Diagramme:
Temperatur mit Sollwert sowie Heizleistung mit der Endpoint-Zeit.

Erwartung in KymoStudio:

- Nach dem Verbinden zeigt das Parameter-Panel `kp`, `ki` und `setpoint °C` mit
  Grenzen und aktuellem Wert.
- `kp` von 0.2 auf 2.0: Die Temperatur erreicht den Sollwert schneller und schwingt
  über. Das Panel zeigt „bestätigt“.
- `setpoint` auf 95: Das Gerät lehnt ab, das Panel zeigt „Gerät behält 45.0“.
- Signal 3 zeigt die maximale Laufzeit eines vollständigen Endpoint-Aufrufs in
  Mikrosekunden, einschließlich Empfang und Versand. Auf dem Board messen.
