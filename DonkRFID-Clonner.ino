#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 2
#define SCL_PIN 3
#define RST_PIN 9

Adafruit_SSD1306 display(128, 64, &Wire, -1);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const int SS_PIN = 10;
const int switchPin = 4;
const int emulateButtonPin = 5;

bool cardDetected = false;
bool emulating = false;
String cardData;

void setup() {
  pinMode(SDA_PIN, OUTPUT);
  pinMode(SCL_PIN, OUTPUT);
  digitalWrite(SDA_PIN, 0);
  digitalWrite(SCL_PIN, 0);
  
  pinMode(switchPin, INPUT);
  pinMode(emulateButtonPin, INPUT);

  Wire.begin();
  display.begin(SSD1306_I2C_ADDRESS, SDA_PIN, SCL_PIN);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (!mfrc522.PCD_Init()) {
    display.println(F("RFID reader initialization failed."));
    display.display();
    while (1);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("RFID Reader / Writer"));
  display.println(F("BVX107"));
  display.display();
  delay(3000);
}

void loop() {
  if (digitalRead(switchPin) == HIGH) {
    if (!cardDetected && !emulating) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("RFID Reader / Writer"));
      display.setCursor(0, 16);
      display.println(F("BVX107"));
      display.setCursor(0, 32);
      display.println(F("Waiting for Card ..."));
      display.display();
      animateDots();
      cardDetected = readCard();
    } else if (emulating) {
      if (emulateCard()) {
        emulating = false;
        cardDetected = false;
        display.clearDisplay();
      }
    }
  }
}

bool readCard() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("Card Detected!"));
    display.setCursor(0, 16);
    display.println(F("Name: "));
    display.display();
    cardData = getCardData();
    return true;
  }
  return false;
}

String getCardData() {
  String data = "";
  for (byte i = 0; i < 4; i++) {
    data += String(mfrc522.uid.uidByte[i], HEX);
  }
  return data;
}

bool emulateCard() {
  display.setCursor(0, 32);
  display.println(F("Emulating Captured Card ..."));
  display.display();
  animateDots();
  
  if (mfrc522.PCD_Init()) {
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    
    for (byte i = 0; i < 4; i++) {
      mfrc522.uid.uidByte[i] = strtoul(cardData.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
    }

    if (mfrc522.MIFARE_SetAccessBits(key, 0x03, 0xFF, 0x08, 0x00)) {
      display.clearDisplay();
      display.setCursor(0, 16);
      display.println(F("Emulated Successfully!"));
      display.display();
      delay(3000);
      return true;
    }
  }
  return false;
}

void animateDots() {
  for (int i = 0; i < 3; i++) {
    display.setCursor(112, 32);
    display.print(".");
    display.display();
    delay(1000);
    display.setCursor(112, 32);
    display.print(" ");
    display.display();
    delay(1200);
  }
}
