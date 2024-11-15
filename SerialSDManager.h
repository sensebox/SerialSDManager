#ifndef SERIALSDMANAGER_H
#define SERIALSDMANAGER_H

#include <SD.h>
#include <SPI.h>
#include <CRC32.h>  // Füge die CRC32-Bibliothek hinzu

class SerialSDManager {
public:
    SerialSDManager(int chipSelectPin);
    void begin();
    void checkForSerialInput();

private:
    const int chipSelect;
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SdFile file;

    static const byte numChars = 50;
    char receivedChars[numChars];
    char tempChars[numChars];

    char cmdMsg[numChars] = {0};
    int cmdId = 0;
    boolean newData = false;

    void recvWithStartEndMarkers();
    void parseData();
    void executeCommand();

void printRoot();
void printFileContent(const char* filename);
void deleteFile(const char* fileName);
void writeFile(const char* fileName, const char* fileContent);
char* findConfigFile();
SdFile getParentDir(const char* filepath, int* index);

};

#endif // SERIALSDMANAGER_H
