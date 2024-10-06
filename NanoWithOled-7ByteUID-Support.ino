#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN         9
#define SS_PIN          10
#define READ_BUTTON     2
#define WRITE_BUTTON    3
#define GREEN_LED       5
#define RED_LED         6

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte storedUID[10];  // 10 byte buffer pre 4-byte alebo 7-byte UID (a viac ak by bolo potrebné)
byte uidSize = 0;    // Premenná pre veľkosť UID
bool uidStored = false;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();
    mfrc522.PCD_Init();

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

    Serial.println(F("Skenovanie PICC pre zobrazenie UID, SAK, typ, a datove bloky..."));
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
    Serial.println(F(" -> [READ MODE] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [READ MODE] <-");
    display.display();

    if (!mfrc522.PICC_IsNewCardPresent()) {
        display.setCursor(0, 20);
        display.print("Nenasiel som kartu :(");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 20);
        display.print("Nepodarilo sa precitat kartu :(");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    // Uloženie veľkosti a UID
    uidSize = mfrc522.uid.size;

    // Kontrola, či je karta so 7-byte UID alebo iným formátom
    if (uidSize == 4 || uidSize == 7) {
        memcpy(storedUID, mfrc522.uid.uidByte, uidSize);
        uidStored = true;

        blinkLED(GREEN_LED, 3);

        Serial.print(F("-> [UID Kod karty] <-"));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("-> [UID Kod karty] <-\n\n");
        for (byte i = 0; i < uidSize; i++) {  // Cyklus pre dynamickú veľkosť UID
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);

            display.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            display.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();
        display.display();

        mfrc522.PICC_HaltA();
    } else {
        Serial.println(F("Nepodporovaný formát UID."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Nepodporovaný UID.");
        display.display();
        blinkLED(RED_LED, 3);
    }
}

void writeRFID() {
    if (!uidStored) {
        Serial.println(F("! Nie je ULOZENE UID !. Prosim prilozte kartu, a ztlacte tlacidlo READ."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("! Neny ULOZENE UID !");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    Serial.println(F(" -> [WRITE MODE] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [WRITE MODE] <-");
    display.display();

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 20);
        display.print("Nenasiel som kartu :(");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    // Zápis UID s ohľadom na jeho veľkosť (4 alebo 7 byte)
    if (mfrc522.MIFARE_SetUid(storedUID, uidSize, true)) {
        Serial.println(F("Uspesne ZAPISANE UID do novej\nkarty"));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("UID Zapisane!");
        display.display();
        blinkLED(GREEN_LED, 3);
    } else {
        Serial.println(F("NEuspesne zapisane UID do\nnovej karty."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("!UID Zapisanie\n    !NEuspesne!");
        display.display();
        blinkLED(RED_LED, 3);
    }

    mfrc522.PICC_HaltA();
}

void blinkLED(int ledPin, int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(200);
    }
}