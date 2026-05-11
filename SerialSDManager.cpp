#include "SerialSDManager.h"

#include "SerialSDManager.h"

SerialSDManager::SerialSDManager(int chipSelectPin) : chipSelect(chipSelectPin) {}

void SerialSDManager::begin() {
    Serial.begin(115200);
    Serial.print("Initializing SD card...");

    if (!SD.begin(chipSelect)) {
        Serial.println("Initialization failed. Card inserted?");
        return;
    }

    Serial.println("SD ok");

    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("Initialization failed. Check your wiring.");
        while (1);
    } else {
        Serial.println("Card is present.");
    }

    if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition. Format the card.");
        while (1);
    }

    if (!root.openRoot(volume)) {
        Serial.println("Could not open root of volume.");
        while (1);
    }
}

void SerialSDManager::checkForSerialInput() {
    recvWithStartEndMarkers();
    if (newData) {
        strcpy(tempChars, receivedChars);
        parseData();
        executeCommand();
        newData = false;
    }
}

void SerialSDManager::recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && !newData) {
        rc = Serial.read();
        if (recvInProgress) {
            if (rc != endMarker) {
                receivedChars[ndx++] = rc;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            } else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        } else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void SerialSDManager::parseData() {
    char *strtokIndx = strtok(tempChars, " ");
    cmdId = atoi(strtokIndx);
    
    // For command 5: first token is filename, rest is content
    if (cmdId == 5) {
        strtokIndx = strtok(NULL, " ");
        if (strtokIndx != nullptr) {
            strcpy(cmdFileName, strtokIndx);
        }
        // Get the rest as content (everything after filename)
        strtokIndx = strtok(NULL, "");  // Empty delimiter = rest of string
        if (strtokIndx != nullptr) {
            strcpy(cmdMsg, strtokIndx);
        }
    } else {
        // For other commands: everything after cmdId is cmdMsg
        strtokIndx = strtok(NULL, "");
        if (strtokIndx != nullptr) {
            strcpy(cmdMsg, strtokIndx);
        }
    }
}

void SerialSDManager::executeCommand() {
    char *configFilename = findConfigFile();

    switch(cmdId) {
        case 1:
            printRoot();
            break;
        case 2:
            printFileContent(cmdMsg);
            break;
        case 3: {
            printFileContent(configFilename);
            break;
        }
        case 4:
            deleteFile(cmdMsg);
            break;
        case 5:
            // Generic file write: <5 filename content>
            writeFile(cmdFileName, cmdMsg);
            break;
        case 7:
            // Write config.cfg and delete all other .cfg files
            writeFile("config.cfg", cmdMsg);
            deleteAllCfgExcept("config.cfg");
            break;
        default:
            Serial.println("Unknown command.");
            break;
    }
}


void SerialSDManager::printRoot() {
    int pathidx = 0;
    SdFile parentdir = getParentDir("/", &pathidx);
    parentdir.ls(LS_SIZE);
    Serial.write("|");
    Serial.write("end");
}

void SerialSDManager::printFileContent(const char* filename) {
    File file = SD.open(filename);
    Serial.write(filename);
    Serial.write("|");

    if (file) {
        // CRC32 calculation (if needed)
        CRC32 crc;
        for (int i = 0; filename[i] != '\0'; i++) {
            crc.update(filename[i]);
        }

        String firstLine = "";
        bool isFirstLine = true;

        while (file.available()) {
            char c = file.read();
            Serial.write(c);
            if (isFirstLine && c != '\n') {
                firstLine += c;
                crc.update(c);
            } else if (c == '\n') {
                isFirstLine = false;
            }
        }

        file.close();
        uint32_t checksum = crc.finalize();
        Serial.write("|");
        Serial.print(checksum, HEX);
        Serial.write("|");
        Serial.write("end");
    } else {
        Serial.println("Error opening the file.");
    }
}

void SerialSDManager::deleteFile(const char* fileName) {
    if (SD.exists(fileName)) {
        if (SD.remove(fileName)) {
            Serial.println("File successfully deleted.");
        } else {
            Serial.println("Error deleting the file.");
        }
    } else {
        Serial.println("The file does not exist.");
    }
}

void SerialSDManager::deleteAllCfgExcept(const char* keepFile) {
    File root = SD.open("/");
    if (!root) {
        Serial.println("Error opening root directory.");
        return;
    }

    // Collect filenames first (can't delete while iterating)
    char filesToDelete[10][50];
    int count = 0;

    while (count < 10) {
        File file = root.openNextFile();
        if (!file) break;

        if (!file.isDirectory()) {
            String name = file.name();
            String lowerName = name;
            lowerName.toLowerCase();
            if (lowerName.indexOf(".cfg") != -1) {
                // Check if this is the file to keep (case-insensitive)
                String keepLower = keepFile;
                keepLower.toLowerCase();
                if (lowerName != keepLower) {
                    name.toCharArray(filesToDelete[count], 50);
                    count++;
                }
            }
        }
        file.close();
    }
    root.close();

    // Now delete collected files
    for (int i = 0; i < count; i++) {
        if (SD.remove(filesToDelete[i])) {
            Serial.print("Deleted old config: ");
            Serial.println(filesToDelete[i]);
        }
    }
}

void SerialSDManager::writeFile(const char* fileName, const char* fileContent) {
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
        Serial.print("Writing to ");
        Serial.println(fileName);
        file.print(fileContent);
        file.close();
        Serial.println("File written successfully.");
    } else {
        Serial.println("Error opening the file for writing.");
    }
}

char* SerialSDManager::findConfigFile() {
    const char* confFileName = nullptr;
    static char buffer[50];
    File root = SD.open("/");
    if (!root) {
        Serial.println("Error opening root directory.");
        return "Error opening root folder";
    }

    while (true) {
        File file = root.openNextFile();
        if (!file) {
            break;
        }

        if (!file.isDirectory()) {
            String name = file.name();
            name.toLowerCase();
            if (name.indexOf(".cfg") != -1) {
                name.toCharArray(buffer, sizeof(buffer));
                file.close();
                root.close();
                return buffer;
            }
        }
        file.close();
    }
    root.close();
    return "No config file found";
}

SdFile SerialSDManager::getParentDir(const char* filepath, int* index) {
    SdFile d1, d2;
    d1.openRoot(volume);

    SdFile* parent = &d1;
    SdFile* subdir = &d2;
    const char* origpath = filepath;

    while (strchr(filepath, '/')) {
        if (filepath[0] == '/') {
            filepath++;
            continue;
        }

        if (!strchr(filepath, '/')) {
            break;
        }

        uint8_t idx = strchr(filepath, '/') - filepath;
        if (idx > 12) {
            idx = 12;
        }
        char subdirname[13];
        strncpy(subdirname, filepath, idx);
        subdirname[idx] = 0;

        subdir->close();
        if (!subdir->open(parent, subdirname, O_READ)) {
            return SdFile();
        }
        filepath += idx;
        parent->close();
        SdFile* t = parent;
        parent = subdir;
        subdir = t;
    }

    *index = (int)(filepath - origpath);
    return *parent;
}
