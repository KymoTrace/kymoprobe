# ESP32: Speicher-Telemetrie (Stacks und Heap)

Drei FreeRTOS-Tasks und der interne Heap als benannte Kurven über `KymoMemory`
(KymoCore 7.3, `KYMO_ENABLE_MEMORY`). Die Firmware listet die Einträge und liest die
Werte in einem Callback; KymoCore ruft selbst keine RTOS-Funktion auf.

| Kanal | Name | Art | Inhalt |
|---|---|---|---|
| 220 | `loop` | Stack-Spitze | Arduino-Loop-Task, 8 KiB |
| 221 | `sensor` | Stack-Spitze | gleichmäßige Last, 2 KiB |
| 222 | `parser` | Stack-Spitze | Rekursion wächst jede Sekunde um eine Ebene, 4 KiB |
| 223 | `internal` | Heap frei | `heap_caps_get_free_size(MALLOC_CAP_INTERNAL)` |
| 224 | `internal` | Heap frei, Minimum | `heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL)` |

Stack-Spitze = Stackgröße minus Watermark. ESP-IDF meldet die Watermark in Byte;
Vanilla-FreeRTOS (z. B. STM32) meldet Wörter, dort im Callback mit
`sizeof(StackType_t)` multiplizieren.

Alle 5 s sendet das Gerät je Eintrag ein MEMORY_INFO (Name, Art, Gesamtgröße).
KymoStudio benennt die Kurven danach und zeigt sie im Diagrammtyp „Speicher“.

## Ausprobieren

```bash
pio run -t upload
```

1. In KymoStudio eine serielle Quelle anlegen (115200 Baud) und starten.
2. Nach spätestens 5 s tragen die Signale ihre Namen.
3. „+ Diagramm“ → „Speicher“: Balken je Task mit Warnschwelle, darunter der Heap.

Bis KymoCore 7.3.0 getaggt ist, baut das Beispiel gegen den KymoCore-Checkout neben
KymoProbe (`lib_deps = KymoCore=symlink://../../../../KymoCore`, Branch `develop`).
