Übersetzt mit ChatGPT. Ich habe es Erfolgreich mit einer TCL TAC-12CHDA eingebunden.

![IMG_0778](https://github.com/user-attachments/assets/9b674e06-41ca-4bcf-b09b-691a5fbd8545)






Externe Komponente für Klimaanlagen von TCL und ähnlichen Modellen für Home Assistant unter Verwendung von ESPHome.
Unterstützt werden Klimaanlagen vom Typ TAC-07CHSA und ähnliche. Leider ist es nahezu unmöglich, im Voraus genau zu sagen, ob eine bestimmte Klimaanlage kompatibel ist, da es enorme Unterschiede in der Ausstattung gibt: Selbst exakt gleiche Modellbezeichnungen können z.B. kein integriertes WLAN-Modul haben, kein Kabel mit USB-Anschluss besitzen oder auf der Steuerplatine gar keinen eingelöteten UART-Anschluss aufweisen.
Trotzdem – mit oder ohne Löten – wurden folgende Klimaanlagen erfolgreich getestet:

Axioma ASX09H1/ASB09H1

Daichi AIR20AVQ1/AIR20FV1

Daichi AIR25AVQS1R-1/AIR25FVS1R-1

Daichi AIR35AVQS1R-1/AIR35FVS1R-1

Daichi DA35EVQ1-1/DF35EV1-1

Dantex RK-12SATI/RK-12SATIE

TCL TAC-07CHSA/TPG-W

TCL TAC-09CHSA/TPG

TCL TAC-09CHSA/DSEI-W

TCL TAC-12CHSA/TPG

Die Komponente erfordert Home Assistant und ESPHome in der Version mindestens 2023.3.0!

Diese Lösung funktioniert AUSSCHLIESSLICH mit Home Assistant und ESPHome. Falls du an anderen Varianten oder Systemen interessiert bist, habe ich auch dazu etwas:
Alternative MQTT-Anbindung

Einen Artikel zum Projekt findest du auf meinem Kanal bei Dzen

Das Ganze funktioniert – sogar stabil. Bugs, die ich gefunden habe, sind behoben, Wünsche, die geäußert wurden, größtenteils umgesetzt. Natürlich nicht alle – ein Sportwagen fehlt leider noch...
Wenn du diese Komponente nutzt, riskierst du keine geistige Gesundheit mehr, aber spontane Glitches könnten trotzdem passieren. Wenn dir so etwas passiert – bitte melde dich bei mir auf Dzen, ich kümmere mich drum.
Eine ausführlichere Beschreibung wird nach und nach auf meinem Dzen-Kanal veröffentlicht – die wichtigsten Infos stelle ich auch hier ein.

Ein Beispiel für die ESPHome-Konfiguration findest du in der Datei TCL-Conditioner.yaml, eine vereinfachte Version in Sample_conf.yaml. Lade sie dir herunter und nutze sie in ESPHome – oder kopiere den Inhalt und ersetze deine Konfiguration damit. Aber denk dran, alle Felder anzupassen! In der Datei findest du Hinweise zu jedem Feld.

Es können zwei Fragen auftauchen: Plattform (also Chip/Modul) und eingebundene Dateien. Ich erkläre beide kurz.

Plattform-Konfiguration
Die Plattform wird genau so konfiguriert, wie es in ESPHome üblich ist. Hier ein Beispiel für den ESP-01S:

yaml
Kopieren
Bearbeiten
esp8266:
  board: esp01_1m
Und so sieht es für das Hommyn-Modul HDN/WFN-02-01 aus dem ersten Artikel aus:

yaml
Kopieren
Bearbeiten
esp32:
  board: esp32-c3-devkitm-1
  framework:
    type: arduino
Die Plattform kann auch über den Haupt-Config-Block eingebunden werden. Ein Beispiel von einem Alpha-Tester für ESP32 WROOM32:

yaml
Kopieren
Bearbeiten
esphome:
  platform: ESP32
  board: nodemcu-32s
Und hier ein Beispiel für den Wemos D1 Mini (ESP12F):

yaml
Kopieren
Bearbeiten
esphome:
  platform: ESP8266
  board: esp12e
Im Grunde ist alles wie gewohnt – die passende Plattform findest du leicht online.

Wichtig: Entferne oder kommentiere die Zeilen für andere Plattformen aus, sonst gibt’s Probleme!

Einbindung externer Dateien
Zum Hinzufügen oder Entfernen bestimmter Teile der Konfiguration habe ich mich für eingebundene Dateien entschieden – ESPHome lädt sie automatisch herunter, wenn der Home Assistant-Server mit dem Internet verbunden ist.
Das hat den Vorteil, dass nicht immer die komplette Konfig bearbeitet werden muss – nur einzelne Teile können ausgetauscht werden, ohne funktionierende Abschnitte zu stören.
Ein weiterer Vorteil: Kein lästiges Kommentieren oder Entkommentieren von ellenlangen Codeblöcken, keine YAML-Formatierungsschmerzen, keine Sorgen wegen Leerzeichen etc. Einfach Links zu Dateien einfügen oder entfernen.
So sieht der Block für eingebundene Dateien aus:

yaml
Kopieren
Bearbeiten
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v – Zeilen müssen exakt hier ausgerichtet sein, sonst gibt’s Fehler!
      - packages/core.yaml # Das Herzstück
      # - packages/leds.yaml
    refresh: 30s
Alle eingebundenen Dateien stehen unter dem Abschnitt files:. Für den Betrieb ist mindestens die folgende Datei notwendig:

yaml
Kopieren
Bearbeiten
- packages/core.yaml # Das Herzstück
Alle weiteren Module sind optional (deren Beschreibung findest du im selben Verzeichnis).
Wichtig: Alle Dateizeilen müssen an der gleichen Position wie die Markierung ausgerichtet sein – sonst stellt ESPHome viele unangenehme Fragen.
So ist es korrekt:

yaml
Kopieren
Bearbeiten
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v – Zeilen müssen exakt hier ausgerichtet sein, sonst gibt’s Fehler!
      - packages/core.yaml # Das Herzstück
      - packages/leds.yaml
    refresh: 30s
So ist es falsch:

yaml
Kopieren
Bearbeiten
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v – Zeilen müssen exakt hier ausgerichtet sein, sonst gibt’s Fehler!
      - packages/core.yaml # Das Herzstück
        - packages/leds.yaml
    refresh: 30s
Wenn du willst, kann ich auch gleich die README.md im Repo für dich anlegen oder übersetzen. Sag einfach Bescheid!








