#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include <Wire.h>  // Knižnica pre I2C komunikáciu

#define SS_PIN 10         // RFID SS pin
#define RST_PIN 9         // RFID RST pin
#define CS_SD_PIN 4       // SD karta CS pin (na D4)

MFRC522 rfid(SS_PIN, RST_PIN);  // Objekt pre RFID
File myFile;                    // Súbor pre SD kartu

void setup() {
  Serial.begin(9600);          // Inicializácia sériovej komunikácie
  SPI.begin();                 // Inicializácia SPI
  Wire.begin();                // Inicializácia I2C

  // Inicializácia RFID modulu
  rfid.PCD_Init();
  Serial.println("Waiting for RFID module...");

  // Inicializácia SD karty
  if (!SD.begin(CS_SD_PIN)) {
    Serial.println("Initialization of SD card failed!");
    return;
  }
  Serial.println("SD card initialized.");

  delay(1000);
}

void loop() {
  // Kontrola prítomnosti karty
  if (!rfid.PICC_IsNewCardPresent()) {
    return;  // Ak nie je nová karta, nič nerob
  }

  // Skontroluj, či môžeš prečítať kartu
  if (!rfid.PICC_ReadCardSerial()) {
    return;  // Ak nie je možné prečítať kartu, nič nerob
  }

  // Otvor súbor na zápis (pridanie údajov)
  myFile = SD.open("rfid.txt", FILE_WRITE); // !! CREATE FILE "rfid.txt" IN YOUR 8 GB SD Card !!
  if (myFile) {
    Serial.print("Card UID: ");
    myFile.print("Card UID: ");

    // Zobrazenie a uloženie UID karty
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
      myFile.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      myFile.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    myFile.println();

    // Zatvor súbor
    myFile.close();
    Serial.println("UID uložené do SD karty.");
  } else {
    // Ak súbor nebolo možné otvoriť, vypíš chybovú správu
    Serial.println("Error opening RFID-Data.txt");
  }

  // Zastavenie komunikácie s kartou
  rfid.PICC_HaltA();
}
