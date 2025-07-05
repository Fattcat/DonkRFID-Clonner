// for 125khz and 13.56mhz
//code
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SD.h>

// === OLED (I2C) ===
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// === RC522 ===
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

// === SD karta ===
#define SD_CS 4

// === RDM6300 (SoftwareSerial) ===
#define RFID_RX 6
#define RFID_TX 7
SoftwareSerial rdmSerial(RFID_RX, RFID_TX);

// === LED diódy ===
#define GREEN_LED 8
#define RED_LED 5

// === Tlačidlá ===
#define READ_BUTTON 2
#define WRITE_BUTTON 3

// === Premenné ===
String lastRDMUID = "";
bool writeMode = false;

void setup() {
  Serial.begin(9600);

  // Inicializácia OLED
  u8g2.begin();
  showOLEDMessage("Inicializujem...");

  // Inicializácia LED a tlačidiel
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(READ_BUTTON, INPUT_PULLUP);
  pinMode(WRITE_BUTTON, INPUT_PULLUP);

  // Inicializácia RDM6300
  rdmSerial.begin(9600);

  // Inicializácia SPI a RC522
  SPI.begin();
  mfrc522.PCD_Init();

  // Inicializácia SD karty
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS)) {
    showOLEDMessage("Chyba SD karty!");
  } else {
    showOLEDMessage("SD karta OK");
  }

  delay(1000);
  clearOLED();
}

void loop() {
  // --- RDM6300 čítanie UID ---
  if (rdmSerial.available() >= 14) {
    byte buffer[14];
    rdmSerial.readBytes(buffer, 14);

    if (buffer[0] == 0x02 && buffer[13] == 0x03) {
      char uid[11];
      for (int i = 0; i < 10; i++) {
        uid[i] = buffer[i + 1];
      }
      uid[10] = '\0';
      String uidStr(uid);

      if (uidStr != lastRDMUID) {
        lastRDMUID = uidStr;
        Serial.print("RDM6300 UID: ");
        Serial.println(uidStr);
        showOLEDMessage("125kHz UID:\n" + uidStr);
        delay(2000);
        clearOLED();
      }
    }
  }

  // --- Tlačidlá ---
  if (digitalRead(READ_BUTTON) == LOW) {
    writeMode = false;
    showOLEDMessage("READ mód");
    delay(1000);
    clearOLED();
  }

  if (digitalRead(WRITE_BUTTON) == LOW) {
    writeMode = true;
    showOLEDMessage("WRITE mód");
    delay(1000);
    clearOLED();
  }

  // --- RC522 čítanie ---
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rc522UID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rc522UID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      rc522UID += String(mfrc522.uid.uidByte[i], HEX);
    }

    Serial.print("RC522 UID: ");
    Serial.println(rc522UID);

    showOLEDMessage("13.56MHz UID:\n" + rc522UID);
    delay(2000);
    clearOLED();

    if (writeMode) {
      if (saveUIDToSD(rc522UID)) {
        digitalWrite(GREEN_LED, HIGH);
        showOLEDMessage("Uložené!");
        delay(1000);
        digitalWrite(GREEN_LED, LOW);
      } else {
        digitalWrite(RED_LED, HIGH);
        showOLEDMessage("Chyba zápisu");
        delay(1000);
        digitalWrite(RED_LED, LOW);
      }
    }

    mfrc522.PICC_HaltA();
  }
}

// === Pomocné funkcie ===

bool saveUIDToSD(String uid) {
  File file = SD.open("uids.txt", FILE_WRITE);
  if (file) {
    file.println(uid);
    file.close();
    return true;
  } else {
    return false;
  }
}

void showOLEDMessage(String msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, msg.c_str());
  u8g2.sendBuffer();
}

void clearOLED() {
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}
