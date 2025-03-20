#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN         9
#define SS_PIN          10
#define GREEN_LED       4
#define RED_LED         6

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte storedUID[10];
byte uidLength = 0;
bool uidStored = false;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();
    mfrc522.PCD_Init();

    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print(F("Proxmark3 emulátor\n\nZadaj príkaz cez\nSerial Monitor."));
    display.display();
    delay(2000);
    Serial.print(F("Proxmark3 Easy DonkRF One Emulator V1.0.0\n-- [+] TYPE -h or --help for more details"));
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim(); // Odstrániť medzery na začiatku a na konci

        if (command == "hf search") {
            hfSearch();
        } else if (command == "hf read") {
            hfRead();
        } else if (command == "hf copy") {
            hfCopy();
        } else if (command == "hf write") {
            hfWrite();
        } else if (command == "") {
            help();
        } else {
            Serial.println(F("Neznámy príkaz. Skúste: hf search, hf read, hf copy, hf write."));
        }
    }
}

void help() {
    Serial.print(F("Arduino Proxmardk3 easy console Emulator\nCommands:\n - [+] hf search - attach RFID card and write this command, then hit ENTER\n - [+] hf copy - for copying which was previously read\n \n - [+] hf write for write"));

void hfSearch() {
    Serial.println(F(" -> [HF SEARCH] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [HF SEARCH] <-");
    display.display();

    if (!mfrc522.PICC_IsNewCardPresent()) {
        Serial.println(F("Žiadna karta nebola nájdená!"));
        display.setCursor(0, 20);
        display.print("Žiadna karta.");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println(F("Nedá sa prečítať kartu!"));
        display.setCursor(0, 20);
        display.print("Chyba pri čítaní");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    uidLength = mfrc522.uid.size;
    memcpy(storedUID, mfrc522.uid.uidByte, uidLength);
    uidStored = true;

    Serial.print(F("UID: "));
    for (byte i = 0; i < uidLength; i++) {
        Serial.print(storedUID[i] < 0x10 ? " 0" : " ");
        Serial.print(storedUID[i], HEX);
    }
    Serial.println();

    display.setCursor(0, 20);
    display.print("UID: ");
    for (byte i = 0; i < uidLength; i++) {
        display.print(storedUID[i] < 0x10 ? " 0" : " ");
        display.print(storedUID[i], HEX);
    }
    display.display();

    mfrc522.PICC_HaltA();
}

void hfRead() {
    if (!uidStored) {
        Serial.println(F("Neexistuje ulozené UID! Zadajte príkaz 'hf search' na načítanie karty."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Neexistuje ulozené UID!");
        display.display();
        return;
    }

    Serial.println(F(" -> [HF READ] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [HF READ] <-");
    display.display();

    Serial.print(F("Čítanie dát z UID: "));
    for (byte i = 0; i < uidLength; i++) {
        Serial.print(storedUID[i] < 0x10 ? " 0" : " ");
        Serial.print(storedUID[i], HEX);
    }
    Serial.println();
    display.setCursor(0, 20);
    display.print("Dáta prečítané!");
    display.display();
}

void hfCopy() {
    if (!uidStored) {
        Serial.println(F("Neexistuje ulozené UID! Zadajte príkaz 'hf search' na načítanie karty."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Neexistuje ulozené UID!");
        display.display();
        return;
    }

    Serial.println(F(" -> [HF COPY] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [HF COPY] <-");
    display.display();

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 20);
        display.print("Nenasiel som kartu :(");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    if (mfrc522.MIFARE_SetUid(storedUID, uidLength, true)) {
        Serial.println(F("Uspesne ZAPISANE UID do novej karty"));
        display.setCursor(0, 20);
        display.print("UID Zapisane!");
        display.display();
        blinkLED(GREEN_LED, 3);
    } else {
        Serial.println(F("NEuspesne zapisane UID do novej karty."));
        display.setCursor(0, 20);
        display.print("Zapisanie neúspešné!");
        display.display();
        blinkLED(RED_LED, 3);
    }
    mfrc522.PICC_HaltA();
}

void hfWrite() {
    if (!uidStored) {
        Serial.println(F("Neexistuje ulozené UID! Zadajte príkaz 'hf search' na načítanie karty."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Neexistuje ulozené UID!");
        display.display();
        return;
    }

    Serial.println(F(" -> [HF WRITE] <-"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(" -> [HF WRITE] <-");
    display.display();

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        display.setCursor(0, 20);
        display.print("Nenasiel som kartu :(");
        display.display();
        blinkLED(RED_LED, 3);
        return;
    }

    if (mfrc522.MIFARE_SetUid(storedUID, uidLength, true)) {
        Serial.println(F("Uspesne ZAPISANE UID do novej karty"));
        display.setCursor(0, 20);
        display.print("UID Zapisane!");
        display.display();
        blinkLED(GREEN_LED, 3);
    } else {
        Serial.println(F("NEuspesne zapisane UID do novej karty."));
        display.setCursor(0, 20);
        display.print("Zapisanie neúspešné!");
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
