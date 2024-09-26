// Code works as normal as "DonkRFID-OledV2.ino" but with new feature for saving UIDs codes to SD Card file.

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>  // Include the SD library

#define RST_PIN         9
#define SS_PIN          10
#define SD_CS_PIN       4  // Chip select pin for SD card
#define READ_BUTTON     2
#define WRITE_BUTTON    3
#define GREEN_LED       5
#define RED_LED         6

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte storedUID[4];
bool uidStored = false;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();
    mfrc522.PCD_Init();

    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println(F("SD card initialization failed!"));
        return; // Stop if the SD card is not initialized
    }
    Serial.println(F("SD card initialized."));

    pinMode(READ_BUTTON, INPUT_PULLUP);
    pinMode(WRITE_BUTTON, INPUT_PULLUP);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    // OLED display setup
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    delay(100);  // Pridaj oneskorenie
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print(F("Priloz RFID kartu\n...\nA ztlac READ Tlacidlo"));
    display.display();
    delay(2000);

    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
    if (digitalRead(READ_BUTTON) == LOW) {
        readRFID();
    }

    if (digitalRead(WRITE_BUTTON) == LOW) {
        writeRFID();
    }
}

void readRFID() {
    Serial.println(F("Read mode activated..."));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Read mode activated...");
    display.display();
    
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 10);
        display.print("No card found.");
        display.display();
        delay(1000);
        return;
    }

    memcpy(storedUID, mfrc522.uid.uidByte, mfrc522.uid.size);
    uidStored = true;

    blinkLED(GREEN_LED, 3);

    Serial.print(F("Card UID:"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Card UID:");
    String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (i > 0) uidString += ":";  // Separate UID bytes with ':'
        uidString += String(mfrc522.uid.uidByte[i], HEX);
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        display.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        display.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    display.display();

    saveUIDToFile(uidString);  // Save UID to file

    mfrc522.PICC_HaltA();
}

void writeRFID() {
    if (!uidStored) {
        Serial.println(F("No UID stored. Please read a card first."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("No UID stored.");
        display.display();
        blinkLED(RED_LED, 3);
        delay(1000);
        return;
    }

    Serial.println(F("Write mode activated..."));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Write mode activated...");
    display.display();
    
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 10);
        display.print("No card found.");
        display.display();
        delay(1000);
        return;
    }

    if (mfrc522.MIFARE_SetUid(storedUID, (byte)4, true)) {
        Serial.println(F("Successfully wrote UID to new card."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("UID written!");
        display.display();
        blinkLED(GREEN_LED, 3);
    } else {
        Serial.println(F("Failed to write UID to new card."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Write failed!");
        display.display();
        blinkLED(RED_LED, 3);
    }

    mfrc522.PICC_HaltA();
}

void saveUIDToFile(String uidString) {
    File dataFile = SD.open("RFID-Data.txt", FILE_APPEND); // Open file in append mode
    if (dataFile) {
        dataFile.println("UID: " + uidString); // Write UID to the file
        dataFile.close(); // Close the file
        Serial.println("UID saved: " + uidString);
    } else {
        Serial.println(F("Error opening RFID-Data.txt"));
    }
}

void blinkLED(int ledPin, int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
    }
}
