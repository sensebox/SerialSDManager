# Command Overview for SerialSDManager

| Command                  | Beschreibung                                                                                                                          |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| `<1 root END>`           | Listet Dateien und Verzeichnisse im Wurzelverzeichnis auf und schreibt diese auf den seriellen Monitor.                               |
| `<2 /logs/test.txt END>` | Liest den Inhalt der angegebenen Datei (`/logs/test.txt`) und gibt ihn auf den seriellen Monitor aus.                                 |
| `<3 config END>`         | Liest den Inhalt der Konfigurationsdatei und gibt ihn auf den seriellen Monitor aus.                                                  |
| `<4 /logs/test.txt END>` | Löscht die angegebene Datei (`/logs/test.txt`) auf der SD-Karte.                                                                      |
| `<5 config.cfg END>`     | Schreibt in die angegebene Datei (`config.cfg`). Der Inhalt, der geschrieben werden soll, wird durch den zweiten Parameter übergeben. |

## Anmerkungen:

- Alle Befehle müssen mit `END>` abgeschlossen werden, um sicherzustellen, dass die Daten korrekt über den seriellen Eingang verarbeitet werden.
- Die Befehle ermöglichen grundlegende Dateisystemoperationen wie das Auflisten, Lesen, Schreiben und Löschen von Dateien.
