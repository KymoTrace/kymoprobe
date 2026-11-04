# ESP32: Live-Variablen aus der ELF-Datei

Die Firmware hat ein paar gewöhnliche globale Variablen, die sich ständig ändern, und
sendet sie **nicht** selbst. KymoStudio lädt die ELF-Datei dieser Firmware, du hakst
Variablen an, und das Gerät tastet sie auf Anfrage selbst mit Gerätezeit ab. Ohne
Debug-Probe, ohne Neukompilieren. Braucht KymoCore 7.3 (`KymoWatch`).

| Variable | Typ | Inhalt |
|---|---|---|
| `loopCounter` | `uint32_t` | Simulationsschritte (100 Hz) |
| `wave` | `float` | Sinus 0,5 Hz |
| `uptimeSeconds` | `double` | Laufzeit in s |
| `alarmActive` | `bool` | Motor über 2800 U/min |
| `motor.rpm` / `motor.mode` / `motor.current` | Struct | Langsames Motormodell, Modus wechselt alle 5 s |

## Was die Firmware dafür tut

```cpp
extern "C" uint8_t _data_start;   // Linkersymbole: .data und .bss im DRAM
extern "C" uint8_t _bss_end;

ranges[0].start  = (uint32_t)(uintptr_t)&_data_start;
ranges[0].length = (uint32_t)((uintptr_t)&_bss_end - (uintptr_t)&_data_start);
const KymoWatchConfig config = { ranges, 1u, buildId, idLength, nullptr, nullptr };
KymoWatch_Init(&watch, &config);
```

In `loop()` reicht sie empfangene Bytes an `KymoWatch_Feed`, sendet Antworten aus
`KymoWatch_NextReply` und jeden fälligen Wert aus `KymoWatch_NextSample` als normalen
DATA-Frame mit Zeitstempel. Mehr ist nicht nötig; die Variablen selbst bleiben
unverändert.

**Sicherheit:** Gelesen wird nur innerhalb von `.data`/`.bss`. Eine Anfrage außerhalb,
falsch ausgerichtet oder mit zu kurzer Periode lehnt das Gerät ab und meldet den Grund.
Geschrieben wird nie.

**Build-ID:** Die ersten 16 Byte des SHA-256 der ELF-Datei, die esptool beim Bauen in
die App-Beschreibung schreibt (`esp_ota_get_app_description()->app_elf_sha256`;
espressif32 6.12 setzt dafür `--elf-sha256-offset`). KymoStudio vergleicht sie mit der
geladenen ELF-Datei und warnt, wenn sie nicht passt.

## Ausprobieren

```bash
pio run -t upload
```

1. In KymoStudio eine serielle Quelle anlegen (115200 Baud, 8N1) und starten.
2. Im Panel „Live-Variablen“ die Datei `.pio/build/esp32-live-variables/firmware.elf`
   wählen. Die Build-ID muss als passend angezeigt werden.
3. `wave` und `motor.rpm` anhaken, Periode z. B. 20 ms. Beide Kurven erscheinen.
4. Board neu starten: Studio setzt die Auswahl von selbst wieder.

Bis KymoCore 7.3.0 getaggt ist, baut das Beispiel gegen den KymoCore-Checkout neben
KymoProbe (`lib_deps = KymoCore=symlink://../../../../KymoCore`, Branch
`feat/live-variables`).
