# Prüfstand — KymoCore/KymoProbe

## Release 7.3.4 — 12.11.2026

KymoProbe verwendet das offizielle KymoCore `v7.3.4`
(`4c50d750dd531b232f649462c845b1a9615c278c`). Die Library-Formatierung ist
übernommen; API, Laufzeitverhalten und Protokoll bleiben unverändert.
Alle bisherigen Tags einschließlich der unveränderten Baseline `v7.3.0`
bleiben erhalten.

| Prüfung | Ergebnis |
|---|---|
| Release-Zuordnung | Sauberes, abgetrenntes Core-Submodul; 25 Verbraucher-Verweise geprüft |
| Core: native Suite und Paket | Bestanden; Archiv-Verbraucher für Uno und ESP32 gebaut |
| Konfigurator | 81 Tests und 108 Untertests bestanden |
| PlatformIO-Auslieferung | 7.3.4 frisch installiert; Quellen und Formatierungskonfiguration abgeglichen; ESP32-Build bestanden |
| Probe: vollständiger nativer Lauf mit KymoStudio | Bestanden; CRC an/aus, 256 Kanäle, 8 Layouts, 42 Fragmentgrößen und vollständige Trace-Sessions mit/ohne Host-Anfragen |
| Probe-Konfiguration, Stress und Feature-Demo | Bestanden |
| Probe-Firmware | 7 von 7 Builds bestanden: ESP32 Standard, Features, Trace, XYZ-TCP-Stress, Uno Multi-Sensor, ESP32 MQTT und STM32 UART |
| Core GitHub CI | Native Tests, Konfigurator Linux/Windows, Paket, Arduino-Lint und ESP-IDF-Build bestanden |

Die Einzelauflistung und lokalen Log-Prüfsummen stehen in
[validation-v7.3.4.json](validation-v7.3.4.json). Die lokalen Logs liegen unter
`.pio/release-7.3.4/`. Mit 7.3.4 wurde keine neue Hardwareprüfung durchgeführt;
die folgenden Hardware-Ergebnisse gehören ausschließlich zu 7.3.3.

## Hardware-Baseline 7.3.3 — 11.11.2026

### Release und tatsächliche Hardwareprüfung

KymoProbe `v7.3.3` verwendet das offizielle KymoCore `v7.3.3`
(`0e195379efd051c20c5db09e3f634847c4f487d3`). Gegenstelle war KymoStudio
2.9.0 auf `feat/ui-polish`, Commit `5148abd75aeb5cfe2132138d38d7e7c692a15478`.
Die Studio-Bedienung erfolgte über den authentifizierten lokalen MCP-Server:
Quellen, Parameter, ELF/Watch, Diagramme, Aufnahmen, Exporte und Replay.
Getestetes Board: ESP32 mit CP210x an **COM4** unter Windows.

Die bestehende Baseline `v7.3.0` bleibt auf `4ad2ba3` erhalten. Text-Signale
und Low-Level-Benchmark behalten ihren Diagnosezweck; die übrigen Anwendungen
verwenden Endpoint und getrennte Feature-Dateien. Metadaten-Requests beim
Verbindungsaufbau sind nicht Teil dieses Releases; Trace wiederholt seine
Metadaten und sein Wörterbuch weiterhin zyklisch.

### Automatisierte Prüfungen

| Prüfung | Ergebnis |
|---|---|
| Core: vollständige native Suite | Bestanden, inklusive optionaler Features, Teilübertragung, Stop/Drain, Metadaten-Wiederholung und gemeinsamer Uhr |
| Konfigurator | 80 Tests, 108 Untertests bestanden; ein Test übersprungen |
| Paket | Archiv/Manifeste, portable Verbraucher und Uno-/ESP32-Beispiele bestanden; PlatformIO 7.3.3 frisch installiert |
| Probe/Studio-Protokollintegration | Vollständig bestanden; 256 Kanäle, 8 Layouts, 42 Fragmentgrößen, CRC-Wiederaufnahme |
| Vollständiges Integrationsbeispiel | Parameter, Watch, Memory, Plot und abgeschlossene Trace-Session; mit/ohne Host-Anfragen und Rückstau bestanden |
| Zusätzliche Probe-Tests | Profile, Stress, Feature-Demo, 8192 Messsignal-Frames/FFT, FreeRTOS-Adapter und spätes Trace-Verbinden bestanden |
| PlatformIO | **44 von 44** Konfigurationen bestanden; Einzelauflistung in der JSON-Datei unten |
| KymoStudio | Vollständiger Lauf nach Neustart: **647 Tests und 321 Untertests bestanden** |
| MCP-Vertragsprüfung | HTTP, stdio und Legacy-Client bestanden, einschließlich Export und Fehlerfällen |
| Struktur/Dokumentation | 316 C/C++-Dateien geprüft; Doxygen ohne Warnungen |

### Live-Prüfungen mit dem offiziellen Core 7.3.3

| Firmware | Über MCP geprüfter Ablauf | Ergebnis |
|---|---|---|
| PI Controller / Plot + Parameter | Drei Parameter entdeckt; `kp=2.0`, Sollwert `55` geschrieben und vom Gerät bestätigt; `95` durch Studio-Grenzwertprüfung abgewiesen; Gerätezeitstempel geprüft | Bestanden; 1003 Messwerte aufgezeichnet, Speicherung abgeschlossen, Aufnahme erneut geöffnet und CSV exportiert |
| Live Variables / Watch | Passendes ELF mit 266 Variablen geladen; `wave` und `loopCounter` abonniert, Periode von 50 auf 20 ms geändert, Counter abbestellt | Bestanden; Build-ID passt, beide Signale verwenden Gerätezeitstempel |
| Memory Telemetry | Kanäle 220–224 für drei Task-Stacks und Heap geprüft; Speicherdiagramm, CSV und PNG erzeugt | Bestanden; fünf gültige Kanäle mit Gerätezeitstempeln |
| Trace UART | Später Verbindungsaufbau, benannte Funktionen, Hotspots und Aufrufbaum; 12 s Capture; CSV/Chrome-Trace; Replay, Pause/Fortsetzen und Seek bis EOF | Bestanden; vollständige Capture-Datei, keine neuen Trace-Lücken während des Aufnahmeabschnitts |

Die Trace-Datei enthält 1096 Batches, 1096 Health-Frames sowie zwei wiederholte
Metadaten-/Wörterbuch-Paare (6 META- und 4 DICTIONARY-Fragmente). Sie ist vollständig
abgeschlossen und nicht abgeschnitten. Beim Einstieg in den laufenden Trace ist
ein `unpaired_or_mismatched_exit` dokumentiert; diese Lücke wächst während der
Aufnahme nicht. Ein gerade offener Funktionsaufruf an der rechten Fenstergrenze
wird von der Laufzeitstatistik ausgeschlossen. Die Quelle meldet außerdem
Resynchronisation (bis 225 verworfene Bytes und einen ungültigen Frame), jedoch
keine CRC-Fehler. Das ist **keine Abnahme eines lückenlosen Gesamtstarts**.
Ein Live-Capture-Stop beendet die Board-Session nicht; Replay erreicht EOF ohne
verifiziertes `SESSION_END`. Ein Seek hinter das Dateiende meldet entsprechend
`target_reached=false`, nicht einen erfolgreich erreichten Zielzeitpunkt.

### Gefundene Fehler und verbleibende Grenzen

- Behoben: UART-TX-Puffer der Trace-Demo auf 1024 Bytes vor dem Portstart gesetzt.
- Behoben in Core 7.3.2: Gerätezeitstempel für Parameter/Watch/Memory wiederhergestellt.
- Behoben in Core 7.3.3: Plot und Steuerdaten verwenden denselben Zeitnullpunkt.
  Der neue Test schlägt auf 7.3.2 fehl und prüft auch Initialisierung bei laufender
  Uhr und uint32-Überlauf. Der Stress-Test prüft nun die echte Geräteuhr einschließlich
  Überlauf, statt fälschlich einen Neustart der Zeit bei jeder Stream-Initialisierung
  vorauszusetzen.
- **Offener Studio-Befund:** Zwei Plot-PNG-Exporte mit der ursprünglichen Darstellung
  liefen in einen Timeout. Die zurückbleibende Auftragssperre blockierte danach
  einen Capture-Start. Nach Neustart der Testinstanz mit
  `QT_QUICK_BACKEND=software` bestanden Plot-PNG, Memory-PNG und Trace-Capture/Replay.
  Die PNGs wurden visuell geprüft. Die ursprüngliche Ursache ist nicht abschließend
  geklärt; es wurde kein pauschaler Darstellungs-Fix in Studio eingebaut.
- Die komplette Studio-Suite bestand im abschließenden Lauf. Vorherige Läufe unter
  paralleler Build-/Hardwarelast hatten einzelne nicht dauerhaft reproduzierbare
  Qt-/TCP-Testfehler; Assertions und Zeitgrenzen wurden nicht abgeschwächt.
- Physisch geprüft ist der ESP32 an COM4. AVR, STM32, ESP32-S3/C3 und Netzwerk-
  Varianten sind gebaut bzw. nativ geprüft; kein Live-Broker-/TCP-/UDP-Test,
  kein Dauertest und keine Abnahme anderer Boards oder echter Sensoren/Aktoren.

Die [maschinenlesbaren Ergebnisse](validation-v7.3.3.json) enthalten alle 44
Build-Konfigurationen, genaue Versionen sowie SHA-256-Werte der lokalen Artefakte.
MCP-Protokoll, Messdateien, Bilder und Einzelprotokolle liegen lokal unter
`.pio/hardware-7.3.1/`; der bestehende Ordnername bleibt erhalten, die endgültigen
Dateien tragen `733`. Build-/Host-Logs stehen unter `.pio/verify-733-*`,
`.pio/verify-native-733.log` und `.pio/verify-studio-post-reboot.log`.
Rohaufnahmen und Zugangsdaten werden nicht ins Repository übernommen.

## Historische Prüfstände

## KymoCore 7.3.0 und modulare Beispiele — 11.11.2026

Der unveränderte Ausgangsstand `4ad2ba3` ist als `v7.3.0` veröffentlicht.
Die danach angepassten Beispiele verwenden durchgängig das offizielle,
unveränderte Core-Submodul `d08fe07` (7.3.0). Die Versionsregeln stehen in
[releases.md](releases.md).

Lokal erfolgreich geprüft:

- `python tools/test_native.py --app ../KymoStudio`: vollständige native
  Core-Suite, Strukturprüfung und Probe-/Studio-Protokollintegration.
- Neues `examples/integration`: tatsächliches setup/loop mit getrenntem
  Testport; gültige/ungültige Parameteränderungen, Watch, Memory, Plot und
  abgeschlossene Trace-Session. Zwei Durchläufe mit/ohne Host-Anfragen,
  Teilübertragungen und Rückstau; Studio-Decoder mit vier Fragmentgrößen.
- PlatformIO: Live Variables, Memory Telemetry, Benchmark bitwise/table sowie
  das neue Integrationsbeispiel auf ESP32 und Uno. Uno enthält kein Trace.
- Versionsprüfung einschließlich negativer Tests für abweichende Tags,
  benachbarte Entwicklungscheckouts und nicht gepinnte Abhängigkeiten.
- Aktualisierte Doxygen-Eingaben und Dokumentation ohne Warnungen.

Keine neue Hardware-Abnahme. Die übrigen Beispiele nutzen weiterhin die
unterstützten Runtime-/Low-Level-/Generator-APIs; die modulare Endpoint-
Integration ist das neue vollständige Einstiegsbeispiel.

## PlatformIO-Paketvorbereitung vom 05.10.2026

Die Library wurde als eigenständiges Paket validiert. Ergänzt sind Lizenztext,
Changelog, Veröffentlichungshinweise, ein portables Beispiel, ausführliche
README-Anleitung mit KymoStudio-Link und drei PNG-Diagramme. Der bisherige
Arbeitsstand einschließlich C++11-/Feature-Umstellung bleibt die Grundlage.
Diese Paketvorbereitung verändert keine öffentliche API und kein Wire-Verhalten.

Frisch erfolgreich ausgeführt:

- `python tools/test_native.py --app ../KymoStudio`: sechs Checker-Tests,
  Single-Return-Prüfung über 40 Quelldateien, interne Headerkonfiguration,
  Featureprofile, beide CRC-Modi und App-Abgleich mit 256 Kanälen, acht Layouts
  und 42 Fragmentgrößen.
- `python tools/test_package.py`: Manifest durch PlatformIO gepackt,
  Archivinhalt/Versionsgleichheit/lokale Dokumentationslinks geprüft,
  portables Beispiel aus dem entpackten Archiv kompiliert und ausgeführt.
  README-Sketch in getrennten Projekten ausschließlich gegen das installierte
  Paketarchiv für Uno und ESP32 gebaut.
- Nach Bereinigung einer Python-Tar-Deprecation-Warnung den Paket-/Hostteil mit
  `python tools/test_package.py --skip-firmware` erneut erfolgreich ausgeführt.
- Die sechs bestehenden Firmwareziele erneut gebaut; Details unten.
- Drei erzeugte Diagramme visuell geprüft; `git diff --check` ohne Inhaltsfehler.

| Aktueller Build | RAM der Demo | Flash der Demo |
|---|---:|---:|
| README aus Archiv / uno | 250 B | 2.884 B |
| README aus Archiv / esp32dev | 21.504 B | 268.109 B |
| Hauptprojekt / nodemcu-32s | 21.504 B | 272.029 B |
| Arduino simple / uno | 258 B | 3.804 B |
| Arduino multidimensional / uno | 268 B | 4.148 B |
| ESP32 serial / esp32dev | 21.504 B | 272.077 B |
| ESP32 MQTT / esp32dev | 44.432 B | 720.465 B |
| STM32 UART C/HAL / nucleo_f401re | 160 B | 10.596 B |

Das Paket liegt lokal unter `.pio/package/KymoCore-6.1.0.tar.gz`, der
zugehörige SHA256-Wert in der benachbarten `.sha256`-Datei. Jeder erneute
Paketlauf aktualisiert das Archiv; vor Upload immer den letzten Stand verwenden.
CI wurde um denselben Paketcheck ergänzt, aber nicht remote ausgeführt.

Nicht Bestandteil dieser Prüfung: Upload in die Registry, Registry-Rendering,
Prüfung des verfügbaren Owner-/Versionsnamens, elektrische Boardtests oder die
komplette App-Testsuite. Der App-Abgleich verwendet den lokalen Nachbarcheckout,
nicht den öffentlich verfügbaren GitHub-Stand. Die Hinweise in
`lib/KymoCore/docs/PUBLISHING.md` beschreiben die verbleibenden Release-Schritte.

## Historischer Prüfstand vom 30.09.2026

Die folgenden Ergebnisse dokumentieren den damaligen Stand. Für die aktuelle
Paketvorbereitung gelten die oben neu ausgeführten Prüfungen.

## Plattformtrennung

STM32-Adapter liegen jetzt unter `examples/stm32/adapters/`, der optionale
Arduino-Print-Adapter unter `examples/common/`. KymoCore enthält keine
SDK-Header, Herstellertypen oder ARDUINO-/STM32-Abzweigungen mehr. C-API und
Wire-Protokoll sind unverändert. Frühere Arduino-C++-Komfortüberladungen werden
durch explizite anwendungsseitige Stream-Adapter ersetzt.

Ein zusätzlicher nativer Build definiert ARDUINO, STM32 und ESP_PLATFORM,
kompiliert dabei ohne SDK und führt die C++-Golden-Vector-Tests aus. Dieser
Build scheiterte vor der Trennung an Print.h und besteht jetzt. Native Tests
und App-Decoder-Abgleich sowie die Builds ESP32-Hauptprojekt, Arduino Uno und
STM32-C/HAL wurden nach dieser Trennung erneut ausgeführt.

## Single-Return-/Common-Überarbeitung

Alle 35 aktiven C/C++-Quelldateien (Library, Anwendung, Beispiele und native
Tests) bestehen `tools/check_embedded_style.py`: höchstens ein Return am Ende
der Funktion, keine Returns in Makros. Sechs Tests prüfen den Strukturchecker
selbst. Es handelt sich um eine Prüfung des hier verwendeten Sprachumfangs,
nicht um einen vollständigen C++-Parser oder eine Safety-Zertifizierung.

Bitmasken/Bitsets, Little-Endian/binary32-Konvertierung und CRC sind im
wiederverwendbaren Header-Modul `lib/KymoCore/src/common/` gebündelt und
werden separat als C99 und C++11 getestet. Dynamische Bitindizes werden vor
Shifts begrenzt. Alle sechs Firmwareziele wurden nach dem Umbau neu gebaut.

Der statische RAM-Bedarf ist unverändert. Flash stieg im einfachen Uno-Beispiel
von 3.968 auf 4.222 Bytes (+254), im ESP32-Hauptprojekt von 272.249 auf 272.321
Bytes (+72). Das ist kein Geschwindigkeitsnachweis; Laufzeit/Stack wurden nicht
auf Hardware gemessen. Ein neuer Fehlerpfadtest belegt: Auch CRC-gültige Frames
mit nicht-endlichen Messwerten verändern bei Ablehnung den Ausgabedatensatz
nicht mehr. Wire-Format und gültige Ergebnisse bleiben unverändert.

## Erfolgreiche lokale Prüfungen

Windows, PlatformIO Core 6.1.19. Plattformversionen sind in den INI-Dateien
festgelegt: espressif32 6.12.0, atmelavr 5.3.0, ststm32 20.0.0.

| Projekt / Ziel | Ergebnis | RAM der gesamten Demo | Flash der gesamten Demo |
| --- | --- | ---: | ---: |
| Hauptprojekt / nodemcu-32s | Build erfolgreich | 21.504 B | 272.321 B |
| Arduino simple / uno | Build erfolgreich | 258 B | 4.222 B |
| Arduino multidimensional / uno | Build erfolgreich | 268 B | 4.268 B |
| ESP32 serial / esp32dev | Build erfolgreich | 21.504 B | 272.445 B |
| ESP32 MQTT / esp32dev | Build erfolgreich | 44.432 B | 720.705 B |
| STM32 UART C/HAL / nucleo_f401re | Build erfolgreich | 160 B | 10.820 B |

Das sind Linkerwerte inklusive Framework, Beispiel und Transport, keine
isolierten Library-Messungen oder maximale Stack-/Heap-Werte. Der C-Kontext
ist im 32-Bit-Host-Test 36 Bytes groß, auf AVR laut ELF-Symboltabelle 32 Bytes.
Für die zwei AVR-Kanäle kommen 8 Bytes Laufzeit-Zeitstempel, 12 Bytes
Kanalkonfiguration und 22 Bytes Callback-Konfiguration hinzu. AVR legt diese
gewöhnlichen const-Tabellen in RAM ab; es wird kein PROGMEM-Zugriff behauptet.

`python tools/test_native.py --app ../KymoStudio` ist erfolgreich:

- Common-Helfer, Single-Return-Regel und deren Checker-Tests;
- C99 und C++11, `-Wall -Wextra -Werror -pedantic`;
- feste Protokollvektoren und CRC-8/ATM-Prüfwert F4 für `123456789`;
- alle 256 Deskriptoren, Ausgabepuffergrenzen, NaN/Inf und beschädigte Frames;
- Init-Validierung, Perioden, Round-Robin, UINT32-Zeitüberlauf, Samplefehler;
- kurze/Null-Writes, Busy, asynchrone Pufferlebensdauer und Treiberfehler;
- voneinander unabhängige Instanzen und optionaler C++-Sender/CDC-Adapter;
- 256 vom echten C-Init/Main-Sender erzeugte Kanäle, alle acht Feldlayouts;
- bytegenauer Abgleich mit Python-Encoder und Verarbeitung durch echten
  KymoStudio-Streamdecoder und Core-Parser bei 42 Fragmentgrößen;
- Wiederaufnahme nach beschädigtem CRC.

Die Gesamtsuite im tatsächlichen KymoStudio-Verzeichnis ist erfolgreich:
**154 passed, 4 subtests passed** mit `.venv/Scripts/python.exe -m pytest -q`.
Die App-Quellen wurden bei dieser ECU-Änderung nicht bearbeitet. Ihr vorhandenes
v6-Protokoll erfüllt den Vertrag bereits. Zwischenzeitlich vorhandene
Windows-Testprobleme waren im abschließend geprüften App-Stand behoben.

`git diff --check` ist ohne Inhaltsfehler. Git meldet lediglich die lokale
LF/CRLF-Normalisierung. Eine CI-Konfiguration für native Tests und die sechs
oben gebauten Firmware-Ziele liegt unter `.github/workflows/embedded.yml`;
die Remote-CI wurde in dieser Sitzung nicht gestartet.

## Reproduktion

```sh
python tools/test_native.py --app ../KymoStudio
pio run -e nodemcu-32s
pio run -d examples/arduino/simple_analog_example -e uno
pio run -d examples/arduino/multi_sensor_example -e uno
pio run -d examples/esp32/serial_example -e esp32dev
pio run -d examples/esp32/wifi_mqtt_example -e esp32dev
pio run -d examples/stm32/uart_example -e nucleo_f401re
```

PlatformIO-Buildlogs liegen lokal in `.pio/*build.log` (nicht versioniert).
Unter Windows ist PlatformIO hier unter
`C:/Users/NoName/.platformio/penv/Scripts/pio.exe` installiert.

## Grenzen

Keine Boards wurden geflasht oder elektrisch geprüft. Serielle Live-Übertragung,
USB-Enumeration, DMA/IRQ-Timing und ein realer MQTT-Broker sind deshalb nicht
hardwareverifiziert. Nano/Mega, ESP32 S3/C3, STM32 F411/F103 und das optionale
USB-Projekt wurden nicht gebaut. Der native PlatformIO-Beispielwrapper wurde
nicht separat gebaut; seine C-Tests wurden direkt mit GCC ausgeführt.

Der Schwerpunkt liegt entsprechend der Präzisierung auf ESP32 und Arduino.
Für eigene ECUs muss der jeweilige Callback-Adapter mit dem tatsächlichen
Treiber geprüft werden. Das Wire-Protokoll selbst bleibt unverändert.

## Referenzen für Build-Metadaten

Die universelle Manifestkonfiguration folgt den offiziellen
[PlatformIO-Manifestangaben](https://docs.platformio.org/en/latest/manifests/library-json/index.html).
Die optionale STM32duino-CDC-Konfiguration orientiert sich am
[offiziellen Core-Buildskript](https://github.com/stm32duino/Arduino_Core_STM32/blob/main/tools/platformio/platformio-build.py).
Das ersetzt keinen Build-/Hardware-Nachweis für dieses optionale Ziel.
