# KymoCore 7.3.0: Integration aller Features

Eigenständiges PlatformIO-Projekt mit der veröffentlichten Library
aus dem auf 7.3.0 gepinnten Submodul `../../lib/KymoCore`. ESP32 zeigt **Plot, Parameter, Watch, Memory und
Trace** gemeinsam. Das Uno-Profil enthält alles außer Trace, um RAM zu sparen.
Eine Sensorbeschaltung ist nicht nötig. Die UART läuft mit **115200 Baud**.

## Einstieg und Dateiaufteilung

Beginne mit [src/main.cpp](src/main.cpp). Dort werden die Configs abgeholt und
die gewünschten Features nacheinander initialisiert. Jeder Status wird geprüft.
Die folgende Kurzfassung zeigt nur die Aufrufreihenfolge:

```cpp
KymoApp_Init(&coreConfig);
KymoApp_PlotInit(&plotConfig);
KymoApp_ParamInit(&paramConfig);
KymoApp_WatchInit(&watchConfig);
KymoApp_MemoryInit(&memoryConfig);
KymoApp_TraceInit(&traceConfig);

// Zyklisch in loop():
KymoApp_TraceHeartbeat(); // Messstelle der Anwendung; zeichnet einen Marker auf.
KymoApp_Main();           // Endpoint: Empfang, Verteilung, Sampling und Versand.
```

`KymoApp_*` sind kleine Projektfunktionen um die veröffentlichten
`KymoEndpoint_*`-APIs. Jedes Feature besitzt seine statische Instanz, Tabellen,
Bindings und Konfiguration in seiner eigenen CPP-Datei. Jeder gleichnamige
Header enthält ausschließlich die Schnittstelle dieses Features, ohne `_setup`.

| Feature | Implementierung / passender Header | Verhalten im Beispiel |
|---|---|---|
| Plot | [kymo_plot.cpp](src/features/kymo_plot.cpp) / [kymo_plot.h](include/features/kymo_plot.h) | Kanal 0, alle 50 ms: Rampe von 0 bis unter `appGain` |
| Parameter | [kymo_param.cpp](src/features/kymo_param.cpp) / [kymo_param.h](include/features/kymo_param.h) | Parameter 100: `gain`, schreibbar von 0 bis 10 |
| Watch | [kymo_watch.cpp](src/features/kymo_watch.cpp) / [kymo_watch.h](include/features/kymo_watch.h) | Ein Watch-Slot, Kanal 150, lesbarer Bereich ausschließlich `appGain` |
| Memory | [kymo_memory.cpp](src/features/kymo_memory.cpp) / [kymo_memory.h](include/features/kymo_memory.h) | Kanal 220, alle 200 ms: synthetische 2048 freie von 4096 Bytes |
| Trace | [kymo_trace.cpp](src/features/kymo_trace.cpp) / [kymo_trace.h](include/features/kymo_trace.h) | Alle 200 ms ein Marker `main cycle` im Kontext `superloop` |

Die gemeinsame Basis in [src/kymo_init.cpp](src/kymo_init.cpp) enthält Endpoint,
RX-/TX-Puffer und Port-Anbindung. Die TX-Puffergröße wird aus den aktivierten
Framegrößen berechnet. Feature-Tabellen gehören in die jeweiligen Feature-Dateien.
Der private Header `src/detail/kymo_internal.h` verbindet sie intern mit der Basis.

Der Hardwareport liegt separat in
[src/port/kymo_port_arduino.cpp](src/port/kymo_port_arduino.cpp): UART starten,
Zeit liefern, begrenzt lesen und schreiben. Die Anwendung nutzt eine einzelne
Superloop; keine parallelen ISR-/RTOS-Aufrufe und kein dynamischer KymoCore-Speicher.

## Was die Features miteinander zeigen

**Parameter:** [app_values.cpp](src/app_values.cpp) enthält `appGain`, anfangs
`1.0f`. In KymoStudio die Parameterliste abrufen und `gain` beispielsweise auf
`2.5` setzen. Die Plot-Amplitude ändert sich damit auf 2.5. Werte außerhalb von
0 bis 10 werden verworfen. Eigene Variablen bindest du über die Parametertabelle
an; Tabelle und Zielvariablen müssen dauerhaft existieren.

**Watch:** Anders als Parameter liest Watch den Wert nur. Mit der zum geflashten
Build gehörenden `.pio/build/<environment>/firmware.elf` die Variable `appGain`
auswählen und als Float32 auf Kanal 150 abonnieren, beispielsweise alle 100 ms.
Erst die Watch-Anforderung des Hosts startet diese Ausgabe. Der erlaubte Bereich
und der Lese-Callback begrenzen Zugriffe auf genau diese Variable. Das Beispiel
konfiguriert keine Watch-Build-ID-Prüfung: ELF und Firmware müssen zusammenpassen.
Der native Test übersetzt die Adresse bei 64-Bit-Hosts in einen festen Demo-Bereich.

**Memory:** `readMemory()` liefert bewusst einen festen Demonstrationswert,
keinen echten Heap-Messwert. Für reale Speicherwerte hier einen passenden
Port-/RTOS-Callback anbinden und Gesamtgröße sowie Speichertyp anpassen.
Die Beschreibung wird beim Start und danach alle 1000 ms übertragen.

**Trace:** Metadaten, Dictionary und Recorder werden vollständig im Trace-Modul
eingerichtet. Die zugehörigen Beschreibungen liegen unter
[features/detail/kymo_trace_records.h](src/features/detail/kymo_trace_records.h).
Neue Symbol-/Kontext-IDs erfordern passende Dictionary-Einträge und einen neu
berechneten `dictionary_hash`. Uhr und Metadaten verwenden 1000 Ticks pro Sekunde.
Session-ID und Build-ID sind feste Demo-Werte; für mehrere Aufzeichnungen passende
Session- und Firmware-Identitäten vergeben.

`KymoApp_TraceHeartbeat()` zeigt eine explizite Messstelle im Anwendungscode.
Weitere Marker oder Laufzeitmessungen werden über eigene Funktionen im
Trace-Feature gekapselt und an den gemessenen Stellen aufgerufen. Der Endpoint
übernimmt Metadaten, Puffer und Versand; er instrumentiert keine Funktionen
automatisch. Das Beispiel erfasst keine Callstacks, ISR- oder Scheduler-Traces.

## Features auswählen und Fehler behandeln

In [include/kymo_user_config.h](include/kymo_user_config.h) lassen sich Parameter,
Watch, Memory und Trace jeweils mit `KYMO_ENABLE_...=0` deaktivieren.
Die zugehörigen Init-Aufrufe und Implementierungen entfallen durch dieselben
Schalter. Plot bleibt das Basisbeispiel. Das Uno-Profil überschreibt Trace auf 0.
Parameter und Watch benötigen den aktivierten Decoder und RX; die gemeinsame
Basis richtet ihre RX-Anbindung ein, solange eines der beiden aktiv ist.

`KYMO_BUSY` ist normaler Rückstau, `KYMO_BAD_SAMPLE` ein übersprungener Messwert.
Bei Konfigurations- oder Transportfehlern stoppt die Beispielschleife weitere
Aufrufe. Den Status `sStatus` im Debugger prüfen oder eigene Fehlerbehandlung
ergänzen. Alle Features vor dem ersten `KymoApp_Main()` initialisieren.

Für einen geordneten Abschluss `KymoApp_RequestStop()` aufrufen und anschließend
`KymoApp_Main()` zyklisch bis `KYMO_IDLE` ausführen, mit Fehlerbehandlung und einem
anwendungsspezifischen Zeitlimit. Dadurch wird auch Trace abgeschlossen. Danach
gegebenenfalls noch die UART-/Treiberwarteschlange leeren lassen, bevor Hardware
abgeschaltet wird. Ein harter Reset schließt die laufende Trace-Session nicht ab.

## Bauen und prüfen

```sh
pio run -e esp32dev -e uno
pio run -e esp32dev -t upload
```

Danach den seriellen Port in KymoStudio mit 115200 Baud öffnen. Es werden
Binärdaten gesendet: keine `Serial.print`-Ausgaben beimischen und den Port nicht
gleichzeitig mit einem Textmonitor öffnen. ESP32 nutzt Hardware-Float, Uno
Software-Float für die Protokollwerte.

Nach `git submodule update --init lib/KymoCore` im Repository-Hauptverzeichnis lässt sich die tatsächliche
Anwendung auch mit dem separaten nativen Testport ausführen:

```sh
python tests/run.py --cxx g++
# Optional zusätzlich mit dem KymoStudio-Decoder prüfen:
python tests/run.py --cxx g++ --app ../../../KymoStudio
```

`--cxx` akzeptiert auch den vollständigen Compilerpfad. Der Test verwendet das
echte `setup()`/`loop()` und alle Feature-Dateien mit der gepinnten Release-Library. Er prüft
Parameteränderungen, ungültige CRCs und Wertebereiche, Watch-Rücklesen,
Plot-Amplitude, Memory-Beschreibungen und eine vollständige Trace-Session.
Fragmentierter RX, Teilübertragungen und TX-Rückstau werden simuliert. Ein zweiter
Durchlauf prüft den Betrieb ohne Host-Anfragen. Die unabhängige Protokollprüfung
stammt aus dem Integrationsbeispiel der installierten Library.

Geprüft: beide PlatformIO-Builds, beide nativen Durchläufe und der KymoStudio-
Decoder mit vier Fragmentgrößen. Nicht auf angeschlossener Hardware ausgeführt.
