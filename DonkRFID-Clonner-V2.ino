#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Zapojenie komponentov:

// MFRC522 RFID:
// SDA -> D10
// SCK -> D13
// MOSI -> D11
// MISO -> D12
// IRQ -> N/A
// GND -> GND
// RST -> D9
// 3.3V -> 3.3V

// OLED displej:
// VCC -> 5V
// GND -> GND
// SDA -> A4
// SCL -> A5

// Tlačidlá:
// Tlačidlo UP -> D2
// Tlačidlo DOWN -> D3
// Tlačidlo ENTER -> D4

// LED diódy:
// Zelená LED -> D5 (cez rezistor)
// Červená LED -> D6 (cez rezistor)

// RFID setup
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button setup
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_ENTER 4

// LED setup
#define LED_GREEN 5
#define LED_RED 6

// Variables
int cursorPosition = 0;
bool readCard = false;
bool writeCard = false;
byte storedData[16]; // Assuming 16 bytes data from RFID card

void setup() {
  // Initialize serial
  Serial.begin(9600);

  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize buttons
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);

  // Initialize LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Display initial menu
  displayMenu();
}

void loop() {
  if (digitalRead(BUTTON_UP) == LOW) {
    cursorPosition = max(0, cursorPosition - 1);
    displayMenu();
    delay(200);
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    cursorPosition = min(1, cursorPosition + 1);
    displayMenu();
    delay(200);
  }

  if (digitalRead(BUTTON_ENTER) == LOW) {
    if (cursorPosition == 0) {
      readRFIDCard();
    } else if (cursorPosition == 1) {
      writeRFIDCard();
    }
    displayMenu();
    delay(200);
  }
}

void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (cursorPosition == 0) {
    display.setCursor(0, 0);
    display.print("--->Read RFID Card");
  } else {
    display.setCursor(0, 0);
    display.print("    Read RFID Card");
  }

  if (cursorPosition == 1) {
    display.setCursor(0, 10);
    display.print("--->Write to RFID Card");
  } else {
    display.setCursor(0, 10);
    display.print("    Write to RFID Card");
  }

  display.display();
}

void readRFIDCard() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Prilož RFID kartu ...");
  display.display();

  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(100);
  }

  // Assuming we're reading a block of data
  MFRC522::StatusCode status = rfid.MIFARE_Read(4, storedData, &rfid.uid.size);
  if (status == MFRC522::STATUS_OK) {
    display.setCursor(0, 20);
    display.print("Karta detekovaná ...");
    display.setCursor(0, 30);
    display.print("dáta uložené");
    digitalWrite(LED_GREEN, HIGH);
  } else {
    display.setCursor(0, 20);
    display.print("Chyba pri čítaní ...");
    digitalWrite(LED_RED, HIGH);
  }
  display.display();
  delay(2000);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

void writeRFIDCard() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Priložte kartu ...");
  display.display();

  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    static int dotCount = 0;
    display.setCursor(0, 10);
    display.print("...");
    display.display();
    delay(1000);
    display.setCursor(0, 10);
    display.print("   ");
    dotCount = (dotCount + 1) % 3;
  }

  MFRC522::StatusCode status = rfid.MIFARE_Write(4, storedData, 16);
  if (status == MFRC522::STATUS_OK) {
    display.setCursor(0, 20);
    display.print("Emulujem do karty ...");
    display.setCursor(0, 30);
    display.print("...");
    digitalWrite(LED_GREEN, HIGH);
  } else {
    display.setCursor(0, 20);
    display.print("Chyba pri zápise ...");
    digitalWrite(LED_RED, HIGH);
  }
  display.display();
  delay(2000);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}
