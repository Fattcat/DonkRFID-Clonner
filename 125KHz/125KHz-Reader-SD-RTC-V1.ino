// tested : Yes
// Works  : Yes, like it should (I think, u can to test it :D)
// Added RTC Module DS3231
// ---------------------------
#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <SoftwareSerial.h>

RTC_DS3231 rtc;

#define RED_LED 5
#define GREEN_LED 4
#define SD_CS_PIN 10

// Nastav SoftSerial pre RFID čítačku
SoftwareSerial rfidSerial(2, 3); // RX, TX -> ! Pozor RDM6300 pin "RX" pripoj na arduino nano pin D3, a pin "TX" na arduino nano pin D2 !

String lastUID = "";  // Na zamedzenie opakovaného výpisu
const int BUFFER_SIZE = 14;
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

bool sd_ready = false;
const char* filename = "rfid.txt";

void setup() {
  Serial.begin(9600);
  rfidSerial.begin(9600);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // Inicializácia RTC
  if (!rtc.begin()) {
    Serial.println("Chyba: RTC modul nie je pripojený!");
    digitalWrite(RED_LED, HIGH); // Zapne červenú LED
    while (1); // Uviazne v nekonečnej slučke
  } else {
    Serial.println("RTC modul pripojený.");
  }

  // Nastavenie času – odkomentuj IBA raz, potom zakomentuj a znova nahraj
  // rtc.adjust(DateTime(2025, 4, 23, 18, 24, 0)); // ← Nastavenie aktuálneho času zakomentuj po prvom nahratí

  // Inicializácia SD karty
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Chyba: SD karta sa nenačítala!");
    digitalWrite(RED_LED, HIGH); // Zostane svietiť
  } else {
    Serial.println("SD karta pripojená.");
    sd_ready = true;
  }

  // Overenie existencie súboru na SD karte
  if (SD.exists(filename)) {
    Serial.println("Súbor existuje. Bude použitý na zápis.");
  } else {
    Serial.println("Súbor neexistuje. Vytváram nový súbor...");
    File newFile = SD.open(filename, FILE_WRITE);
    if (newFile) {
      newFile.println("Záznamy RFID kariet:");
      newFile.close();
      Serial.println("Súbor vytvorený.");
    } else {
      Serial.println("Chyba pri vytváraní súboru.");
      digitalWrite(RED_LED, HIGH); // Zapne červenú LED, ak súbor sa nedá vytvoriť
      while (1);
    }
  }

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (rfidSerial.available() > 0) {
    int ssvalue = rfidSerial.read();
    if (ssvalue == -1) return;

    // Začiatok a koniec RFID dát
    if (ssvalue == 2) { // Start byte
      buffer_index = 0;
    } else if (ssvalue == 3) { // End byte
      if (buffer_index >= 10 && buffer_index <= 14) { // Skontroluj, že UID má správnu dĺžku (8 bajtov)
        process_tag();
      } else {
        Serial.println("Chyba: Nesprávna dĺžka UID.");
      }
    }

    if (buffer_index < BUFFER_SIZE) {
      buffer[buffer_index++] = ssvalue;
    }
  }
}

void process_tag() {
  // Predpokladáme, že UID začína na 3. pozícii (po štartovacom byte)
  // Prvé 8 bajtov od pozície 3 je UID
  char* msg_data_tag = (char*)(buffer + 3);

  // Získanie aktuálneho času
  DateTime now = rtc.now();
  String timeString = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " +
                      String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  // Detekcia typu karty
  String card_type = "Unknown";
  if (msg_data_tag[0] == '0' && msg_data_tag[1] == '0') {
    card_type = "EM4100";
  } else if (msg_data_tag[0] == 'T' && msg_data_tag[1] == '5') {
    card_type = "T5577";
  }

  // Zobrazenie v Serial monitore
  Serial.println("-------------------");
  Serial.print("UID: ");
  for (int i = 0; i < 8; ++i) {  // Predpokladáme, že UID je dĺžky 8
    Serial.print(msg_data_tag[i]);
  }
  Serial.println();
  Serial.print("Card type : ");
  Serial.println(card_type);
  Serial.print("Cas : ");
  Serial.println(timeString);
  Serial.println("-------------------");

  // Zápis do súboru na SD kartu
  if (sd_ready) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.println("-------------------");
      file.print("UID: ");
      for (int i = 0; i < 8; ++i) {
        file.print(msg_data_tag[i]);
      }
      file.println();
      file.print("Card type : ");
      file.println(card_type);
      file.print("Cas : ");
      file.println(timeString);
      file.println("-------------------");
      file.close();

      // Blikanie LED 5x rýchlo po úspešnom zápise
      for (int i = 0; i < 5; i++) {
        digitalWrite(GREEN_LED, HIGH);
        delay(100);
        digitalWrite(GREEN_LED, LOW);
        delay(100);
      }
    } else {
      Serial.println("Chyba pri zápise do súboru.");
      digitalWrite(RED_LED, HIGH); // Indikácia chyby zápisu
    }
  }
}
