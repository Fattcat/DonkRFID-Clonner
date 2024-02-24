#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define READ_BUTTON_PIN 2
#define WRITE_BUTTON_PIN 3
#define GREEN_LED_PIN 4
#define RED_LED_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  pinMode(READ_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WRITE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (digitalRead(READ_BUTTON_PIN) == LOW) {
    if (readRFID()) {
      flashLED(GREEN_LED_PIN);
    } else {
      flashLED(RED_LED_PIN);
    }
  }

  if (digitalRead(WRITE_BUTTON_PIN) == LOW) {
    if (writeRFID()) {
      flashLED(GREEN_LED_PIN);
    } else {
      flashLED(RED_LED_PIN);
    }
  }
}

bool readRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Read RFID data here
  return true;
}

bool writeRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Write RFID data here
  return true;
}

void flashLED(int pin) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }
}
