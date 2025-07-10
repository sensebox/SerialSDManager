#include <SerialSDManager.h>

SerialSDManager sdManager(28); // Pass the chip select pin for your SD card

void setup() {
    sdManager.begin(); // Initialize the SD card and other setup
}

void loop() {
    sdManager.checkForSerialInput(); // Non-blocking check for serial input
    // Your other loop code here
}