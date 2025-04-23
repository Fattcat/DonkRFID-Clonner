// tested : Yes
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
SoftwareSerial rfidSerial(2, 3); // RX, TX

String lastUID = "";  // na zamedzenie opakovaného výpisu

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
    digitalWrite(RED_LED, HIGH);
    while (1);
  }

  // Nastavenie času – odkomentuj IBA raz nahraj kód,a potom zakomentuj a znova nahraj
  // rtc.adjust(DateTime(2025, 4, 23, 18, 24, 0)); // ← Aktuálny čas zakomentuj po prvom nahratí

  // Inicializácia SD karty
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Chyba: SD karta sa nenačítala!");
    digitalWrite(RED_LED, HIGH); // Zostane svietiť
  }

  Serial.println("Systém pripravený.");
}

void loop() {
  // 1. Zobraz aktuálny čas
  DateTime now = rtc.now();
  Serial.print("Čas: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // 2. RFID načítanie (ak je dostupné niečo na porte)
  if (rfidSerial.available() > 0) {
    String uid = "";
    while (rfidSerial.available() > 0) {
      char ch = rfidSerial.read();
      uid += ch;
      delay(5);
    }

    uid.trim(); // odstráni medzery
    if (uid != lastUID && uid.length() > 0) {
      Serial.print("Načítaná karta: ");
      Serial.println(uid);

      // Zablikaj zelenou LED
      digitalWrite(GREEN_LED, HIGH);
      delay(300);
      digitalWrite(GREEN_LED, LOW);

      lastUID = uid; // uloží poslednú UID
    }
  }

  delay(1000);
}
