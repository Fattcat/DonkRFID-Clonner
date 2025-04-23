// tested : No
// Added RTC Module
// ---------------------------

#include <SoftwareSerial.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

#define RFID_RX 6
#define RFID_TX 8
#define SD_CS 10

#define GREEN_LED 4
#define RED_LED 5

const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;

SoftwareSerial ssrfid(RFID_RX, RFID_TX);
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

RTC_DS3231 rtc;
bool sd_ready = false;
const char* filename = "rfid-data.txt";

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.begin(9600);
  ssrfid.begin(9600);
  ssrfid.listen();

  // RTC inicializácia
  if (!rtc.begin()) {
    Serial.println("⚠️ Chyba: RTC modul neodpovedá!");
    blinkLED(RED_LED, 5, 300);
  }

  // Prvé nastavenie času - len RAZ (potom zmaž alebo zakomentuj)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("⚠️ Chyba: SD karta sa nenačítala.");
    digitalWrite(RED_LED, HIGH);
    while (true);
  }

  sd_ready = true;
  Serial.println("✅ SD karta inicializovaná.");

  if (SD.exists(filename)) {
    Serial.println("Súbor existuje. Bude použitý na zápis.");
  } else {
    Serial.println("Súbor neexistuje. Vytváram nový...");
    File newFile = SD.open(filename, FILE_WRITE);
    if (newFile) {
      newFile.println("Záznamy RFID kariet:");
      newFile.close();
    } else {
      Serial.println("⚠️ Chyba: Nepodarilo sa vytvoriť súbor.");
      digitalWrite(RED_LED, HIGH);
      while (true);
    }
  }

  Serial.println("Čakám na RFID kartu...");
}

unsigned long lastRFIDTime = 0;

void loop() {
  if (ssrfid.available() > 0) {
    lastRFIDTime = millis();
    digitalWrite(GREEN_LED, HIGH);
    bool call_extract_tag = false;
    int ssvalue = ssrfid.read();
    if (ssvalue == -1) return;

    if (ssvalue == 2) buffer_index = 0;
    else if (ssvalue == 3) call_extract_tag = true;

    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("⚠️ Chyba: Buffer overflow!");
      blinkLED(RED_LED, 2, 500);
      digitalWrite(GREEN_LED, LOW);
      buffer_index = 0;
      return;
    }

    buffer[buffer_index++] = ssvalue;

    if (call_extract_tag && buffer_index == BUFFER_SIZE) {
      digitalWrite(GREEN_LED, LOW);
      process_tag();
    }
  }

  // Detekcia nepripojeného čítača: ak nič neprišlo za 10 sekúnd
  if (millis() - lastRFIDTime > 10000) {
    Serial.println("⚠️ Upozornenie: Žiadne údaje z RDM6300 (skontroluj pripojenie).");
    blinkLED(RED_LED, 2, 100);
    lastRFIDTime = millis(); // reset pre opakovanú kontrolu
  }
}

void process_tag() {
  char* msg_data_tag = (char*)(buffer + 3);

  Serial.println("-------------------");
  Serial.print("UID: ");
  for (int i = 0; i < DATA_TAG_SIZE; ++i) {
    Serial.print(msg_data_tag[i]);
  }

  String card_type = (msg_data_tag[0] == '0' && msg_data_tag[1] == '0') ? "EM4100" : "Unknown";

  Serial.print(" , Card Type: ");
  Serial.println(card_type);

  DateTime now = rtc.now();
  Serial.print("Čas: ");
  Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE));
  Serial.print(" ");
  Serial.println(now.timestamp(DateTime::TIMESTAMP_TIME));

  Serial.println("-------------------");

  if (sd_ready) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.print("UID: ");
      for (int i = 0; i < DATA_TAG_SIZE; ++i) {
        file.print(msg_data_tag[i]);
      }
      file.print(" , Card Type: ");
      file.print(card_type);
      file.print(" , Timestamp: ");
      file.print(now.timestamp(DateTime::TIMESTAMP_DATE));
      file.print(" ");
      file.println(now.timestamp(DateTime::TIMESTAMP_TIME));
      file.close();
      blinkLED(GREEN_LED, 3, 100);
    } else {
      Serial.println("⚠️ Chyba: Nepodarilo sa otvoriť súbor.");
      blinkLED(RED_LED, 3, 100);
    }
  }
}

void blinkLED(int pin, int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(duration);
    digitalWrite(pin, LOW);
    delay(duration);
  }
}