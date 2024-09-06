#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN         9          // Reset pin for RFID
#define SS_PIN          10         // Slave Select (SS) pin for RFID (SDA pin on your module)
#define READ_BUTTON     2          // Pin for the READ button
#define WRITE_BUTTON    3          // Pin for the WRITE button
#define GREEN_LED       4          // Pin for the GREEN LED
#define RED_LED         5          // Pin for the RED LED

#define SCREEN_WIDTH    128        // OLED display width, in pixels
#define SCREEN_HEIGHT   64         // OLED display height, in pixels
#define OLED_RESET      -1         // Reset pin # (or -1 if sharing Arduino reset pin)


// CONECTION
// Piny na Arduino Nano:

// SCK (SPI) → pin 13 na Nano
// MOSI (SPI) → pin 11 na Nano
// MISO (SPI) → pin 12 na Nano
// SDA (SS) → pin 10 na Nano (označený ako SDA na RFID, ale funguje ako SS)
// RST → pin 9 na Nano
// IRQ → nepoužíva sa (môžeš ho ponechať nezapojený)
// OLED displej:
// 
// SDA (I2C) → A4 na Nano
// SCL (I2C) → A5 na Nano
// LED a tlačidlá:
// 
// Tlačidlá sú pripojené na pin 2 (READ) a pin 3 (WRITE), pričom používame vnútorné pull-up rezistory.
// Zelená LED na pin 4 a červená LED na pin 5, každá s odporom okolo 220 Ω.


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED instance

// Variable to store the UID of the first card
byte storedUID[4];
bool uidStored = false;

void setup() {
    Serial.begin(9600);         // Initialize serial communications with the PC
    while (!Serial);            // Wait for Serial monitor (needed on ATMEGA32U4 based boards)
    SPI.begin();                // Init SPI bus
    mfrc522.PCD_Init();         // Init MFRC522

    // Pin setup for buttons and LEDs
    pinMode(READ_BUTTON, INPUT_PULLUP);  // Button for read mode (using internal pull-up)
    pinMode(WRITE_BUTTON, INPUT_PULLUP); // Button for write mode (using internal pull-up)
    pinMode(GREEN_LED, OUTPUT);          // Green LED
    pinMode(RED_LED, OUTPUT);            // Red LED

    // OLED display setup
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Loop forever if OLED fails to initialize
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
    // Check if the READ button is pressed
    if (digitalRead(READ_BUTTON) == LOW) {
        readRFID();
    }
    
    // Check if the WRITE button is pressed
    if (digitalRead(WRITE_BUTTON) == LOW) {
        writeRFID();
    }
}

// Function to read RFID and store UID
void readRFID() {
    Serial.println(F("Read mode activated..."));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Read mode activated...");
    display.display();
    
    // Reset if no new card is present
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 10);
        display.print("No card found.");
        display.display();
        delay(1000);
        return;
    }

    // Store the UID of the card
    memcpy(storedUID, mfrc522.uid.uidByte, mfrc522.uid.size);
    uidStored = true;

    // Blink the green LED 3 times to indicate successful read
    blinkLED(GREEN_LED, 3);

    // Print UID to serial
    Serial.print(F("Card UID:"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        display.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        display.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    display.display();

    // Halt the card
    mfrc522.PICC_HaltA();
}

// Function to write stored UID to a new card
void writeRFID() {
    if (!uidStored) {
        Serial.println(F("No UID stored. Please read a card first."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("No UID stored.");
        display.display();
        blinkLED(RED_LED, 3); // Blink red LED 3 times to indicate failure
        delay(1000);
        return;
    }

    Serial.println(F("Write mode activated..."));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Write mode activated...");
    display.display();
    
    // Reset if no new card is present
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 10);
        display.print("No card found.");
        display.display();
        delay(1000);
        return;
    }

    // Write the stored UID to the new card
    if (mfrc522.MIFARE_SetUid(storedUID, (byte)4, true)) {
        Serial.println(F("Successfully wrote UID to new card."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("UID written!");
        display.display();
        blinkLED(GREEN_LED, 3); // Blink green LED 3 times to indicate success
    } else {
        Serial.println(F("Failed to write UID to new card."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Write failed!");
        display.display();
        blinkLED(RED_LED, 3);   // Blink red LED 3 times to indicate failure
    }

    // Halt the card
    mfrc522.PICC_HaltA();
}

// Function to blink an LED a specific number of times
void blinkLED(int ledPin, int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
    }
}
