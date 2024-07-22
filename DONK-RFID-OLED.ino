// code for copy RFID card and EMULATE to other RFID card.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RST_PIN         9
#define SS_PIN          10
MFRC522 rfid(SS_PIN, RST_PIN);

#define BUTTON1_PIN 2
#define BUTTON2_PIN 3

byte readCard[16]; // Assuming card data length is 16 bytes

void setup() {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
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
  if (digitalRead(BUTTON1_PIN) == LOW) {
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

  if (digitalRead(BUTTON2_PIN) == LOW) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Attach the card");
    display.display();
    
    while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      // Wait for an RFID card to be present
    }
    
    bool success = true;
    for (byte i = 0; i < 16; i++) {
      // Emulate the read data to the new card
      if (!rfid.MIFARE_WriteBlock(i, &readCard[i], 16)) {
        success = false;
        break;
      }
    }
    
    display.clearDisplay();
    if (success) {
      display.print("Successfully emulated to card");
    } else {
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
}
