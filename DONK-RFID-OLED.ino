#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RST_PIN         15
#define SS_PIN          5
MFRC522 rfid(SS_PIN, RST_PIN);

#define BUTTON1_PIN 4
#define BUTTON2_PIN 13

byte readCard[16]; // Assuming card data length is 16 bytes
bool cardRead = false;
bool button1State = false;
bool button2State = false;
unsigned long lastButtonPress1 = 0;
unsigned long lastButtonPress2 = 0;
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

void setup() {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  
  Serial.begin(115200);
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("RFID Copy");
  display.setCursor(0, 10);
  display.print("/Emulator");
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Button1 - Read RFID");
  display.setCursor(0, 10);
  display.print("Button2 - Emulate to new RFID");
  display.display();
}

void loop() {
  bool readingButton1 = digitalRead(BUTTON1_PIN) == LOW;
  bool readingButton2 = digitalRead(BUTTON2_PIN) == LOW;

  if (readingButton1 && !button1State && (millis() - lastButtonPress1 > debounceDelay)) {
    lastButtonPress1 = millis();
    button1State = true;
    handleButton1();
  } else if (!readingButton1) {
    button1State = false;
  }

  if (readingButton2 && !button2State && (millis() - lastButtonPress2 > debounceDelay)) {
    lastButtonPress2 = millis();
    button2State = true;
    handleButton2();
  } else if (!readingButton2) {
    button2State = false;
  }
}

void handleButton1() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Waiting for RFID Card ...");
  display.display();
  
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    // Wait for an RFID card to be present
  }
  
  display.setCursor(0, 10);
  display.print("Detected Card !");
  display.display();
  
  // Read card data
  for (byte i = 0; i < 16; i++) {
    readCard[i] = rfid.uid.uidByte[i % rfid.uid.size];
  }
  cardRead = true;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Card has been read");
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Button1 - Read RFID");
  display.setCursor(0, 10);
  display.print("Button2 - Emulate to new RFID");
  display.display();
}

void handleButton2() {
  if (!cardRead) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("No card data to emulate");
    display.display();
    delay(2000);
    return;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Attach the card");
  display.display();
  
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    // Wait for an RFID card to be present
  }
  
  // Attempt to clear each block in the card
  bool success = clearCard();
  
  if (success) {
    // Write new data to the card
    success = writeCard();
  }
  
  display.clearDisplay();
  if (success) {
    display.setCursor(0, 0);
    display.print("Successfully emulated");
    display.setCursor(0, 10);
    display.print("to card");
  } else {
    display.setCursor(0, 0);
    display.print("Emulating failed!");
  }
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Button1 - Read RFID");
  display.setCursor(0, 10);
  display.print("Button2 - Emulate to new RFID");
  display.display();
}

bool clearCard() {
  byte sector;
  byte blockAddr;
  MFRC522::StatusCode status;
  MFRC522::MIFARE_Key key;
  
  // Use default key
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  for (sector = 0; sector < 16; sector++) {
    if (!authSector(sector, &key)) {
      return false;
    }
    for (byte block = 0; block < 4; block++) {
      byte buffer[16] = {0}; // Buffer to hold the data
      status = rfid.MIFARE_Write(sector * 4 + block, buffer, 16);
      if (status != MFRC522::STATUS_OK) {
        return false;
      }
    }
  }
  return true;
}

bool writeCard() {
  byte sector;
  byte blockAddr;
  MFRC522::StatusCode status;
  MFRC522::MIFARE_Key key;
  
  // Use default key
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  for (sector = 0; sector < 16; sector++) {
    if (!authSector(sector, &key)) {
      return false;
    }
    for (byte block = 0; block < 4; block++) {
      byte buffer[16] = {0}; // Assuming the readCard data fits into a 16-byte block
      memcpy(buffer, &readCard[0], 16); // Copy the data to buffer
      status = rfid.MIFARE_Write(sector * 4 + block, buffer, 16);
      if (status != MFRC522::STATUS_OK) {
        return false;
      }
    }
  }
  return true;
}

bool authSector(byte sector, MFRC522::MIFARE_Key *key) {
  MFRC522::StatusCode status;
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector * 4, key, &rfid.uid);
  if (status != MFRC522::STATUS_OK) {
    return false;
  }
  return true;
}
