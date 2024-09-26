#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definícia pinov pre RFID čítačku
#define RST_PIN 9
#define SS_PIN 10

// Definícia OLED displeja - šírka a výška
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RFID čítačka
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // Inicializácia sériového monitora
  Serial.begin(9600);
  while (!Serial);

  // Inicializácia OLED displeja
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED displej sa nenašiel"));
    for (;;); // Zastaví program, ak OLED displej nie je detekovaný
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("RFID cakanie..."));
  display.display();

  // Inicializácia RFID čítačky
  SPI.begin();      
  rfid.PCD_Init();  
  Serial.println(F("Pripravene na citanie RFID tagu"));
}

void loop() {
  // Skontroluj, či je karta priložená
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Vyčisti OLED displej
  display.clearDisplay();
  display.setCursor(0, 0);

  // Zobrazenie typu karty
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  String cardType = rfid.PICC_GetTypeName(piccType);
  Serial.print(F("Typ karty: "));
  Serial.println(cardType);
  display.print(F("Typ: "));
  display.println(cardType);

  // Získanie UID karty
  String uid = "";
  Serial.print(F("UID Karty: "));
  display.print(F("UID: "));
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.println();
  display.println(uid);

  // Zobrazenie veľkosti pamäte
  Serial.print(F("Velkost pamate: "));
  int memorySize = getMemorySize(piccType);
  Serial.println(memorySize);
  display.print(F("Pamet: "));
  display.println(memorySize);

  // Výpis všetkých blokov pamäte karty (voliteľné - zobrazí iba na serial monitor)
  Serial.println(F("Citane bloky pamate:"));
  for (byte block = 0; block < 64; block++) {
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status = rfid.MIFARE_Read(block, buffer, &size);
    if (status == MFRC522::STATUS_OK) {
      Serial.print(F("Blok ")); Serial.print(block); Serial.print(F(": "));
      for (byte i = 0; i < 16; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
      }
      Serial.println();
    } else {
      Serial.print(F("Chyba citania bloku ")); Serial.println(block);
    }
  }

  // Odstránenie karty
  rfid.PICC_HaltA();
  display.display();
  delay(1000); // Počkajte jednu sekundu pred ďalším čítaním
}

// Funkcia na vrátenie veľkosti pamäte podľa typu karty
int getMemorySize(MFRC522::PICC_Type piccType) {
  switch (piccType) {
    case MFRC522::PICC_TYPE_MIFARE_MINI:
      return 320; // MIFARE Mini (320 bajtov)
    case MFRC522::PICC_TYPE_MIFARE_1K:
      return 1024; // MIFARE 1K (1 KB)
    case MFRC522::PICC_TYPE_MIFARE_4K:
      return 4096; // MIFARE 4K (4 KB)
    case MFRC522::PICC_TYPE_MIFARE_UL:
      return 64; // MIFARE Ultralight (64 bajtov)
    default:
      return 0; // Neznáma veľkosť pamäte
  }
}