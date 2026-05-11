# Command Overview for SerialSDManager

| Command                  | Beschreibung                                                                                                                          |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| `<1 root>` | Listet Dateien und Verzeichnisse im Wurzelverzeichnis auf und schreibt diese auf den seriellen Monitor. |
| `<2 /logs/test.txt>` | Liest den Inhalt der angegebenen Datei (`/logs/test.txt`) und gibt ihn auf den seriellen Monitor aus. |
| `<3 config>` | Liest den Inhalt der Konfigurationsdatei (erste gefundene .cfg Datei) und gibt ihn auf den seriellen Monitor aus. |
| `<4 /logs/test.txt>` | Löscht die angegebene Datei (`/logs/test.txt`) auf der SD-Karte. |
| `<5 dateiname.txt [fileContent]>` | Schreibt eine beliebige Datei mit dem angegebenen Namen und Inhalt. |
| `<7 [fileContent]>` | Schreibt `config.cfg` mit dem angegebenen Inhalt und löscht danach alle anderen `.cfg` Dateien auf der SD-Karte. |

## Anmerkungen:

- Alle Befehle werden mit `<` gestartet und mit `>` beendet.
- Die Befehle ermöglichen grundlegende Dateisystemoperationen wie das Auflisten, Lesen, Schreiben und Löschen von Dateien.
- Befehl 7 ist speziell für die Config-Aktualisierung gedacht: Er schreibt zuerst die neue Config und räumt dann alte Config-Dateien auf.
