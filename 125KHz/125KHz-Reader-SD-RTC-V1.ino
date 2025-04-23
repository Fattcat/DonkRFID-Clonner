// tested : No
// Added RTC Module
// ---------------------------

#include <SoftwareSerial.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>  // Podporuje DS1307 aj DS3231

#define RFID_RX 6
#define RFID_TX 8
#define SD_CS 10
#define GREEN_LED 4
#define RED_LED 5

const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;

SoftwareSerial ssrfid(RFID_RX, RFID_TX);
RTC_DS3231 rtc;  // Zmena: použitie DS3231 namiesto DS1307
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

void setup() {
  Serial.begin(9600);
  ssrfid.begin(9600);
  ssrfid.listen();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("⚠️ Chyba: RTC DS3231 neodpovedá.");
    signalError(3);
  }

  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC DS3231 stratil napájanie. Nastavujem čas...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // nastav len raz
  }

  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("⚠️ Chyba: SD karta nie je pripojená.");
    signalError(2);
    while (true);
  }

  File file = SD.open("RFID-Data.txt", FILE_WRITE);
  if (!file) {
    Serial.println("⚠️ Chyba: Nepodarilo sa otvoriť súbor.");
    signalError(5);
  } else {
    file.close();
    Serial.println("✅ SD karta a súbor pripravený.");
  }

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (ssrfid.available() > 0) {
    digitalWrite(GREEN_LED, HIGH);
    bool call_extract_tag = false;
    int ssvalue = ssrfid.read();

    if (ssvalue == -1) return;

    if (ssvalue == 2) buffer_index = 0;
    else if (ssvalue == 3) call_extract_tag = true;

    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("⚠️ Buffer overflow!");
      digitalWrite(GREEN_LED, LOW);
      signalError(4);
      return;
    }

    buffer[buffer_index++] = ssvalue;

    if (call_extract_tag && buffer_index == BUFFER_SIZE) {
      process_tag();
    }
  }
}

void process_tag() {
  char* msg_data_tag = (char*)(buffer + 3);
  char uid[DATA_TAG_SIZE + 1] = {0};
  for (int i = 0; i < DATA_TAG_SIZE; i++) {
    uid[i] = msg_data_tag[i];
  }

  DateTime now = rtc.now();

  Serial.println("-------------------");
  Serial.print("UID: ");
  Serial.println(uid);
  Serial.print("Čas: ");
  Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE));
  Serial.print(" ");
  Serial.println(now.timestamp(DateTime::TIMESTAMP_TIME));
  Serial.println("-------------------");

  File file = SD.open("RFID-Data.txt", FILE_WRITE);
  if (file) {
    file.print("UID: ");
    file.print(uid);
    file.print(", Time: ");
    file.print(now.timestamp(DateTime::TIMESTAMP_DATE));
    file.print(" ");
    file.println(now.timestamp(DateTime::TIMESTAMP_TIME));
    file.close();
    digitalWrite(GREEN_LED, LOW);
    blinkLED(GREEN_LED, 3, 100);
  } else {
    digitalWrite(GREEN_LED, LOW);
    signalError(5);
  }

  buffer_index = 0;
}

void signalError(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(RED_LED, HIGH);
    delay(500);
    digitalWrite(RED_LED, LOW);
    delay(500);
  }
}

void blinkLED(int pin, int count, int speed) {
  for (int i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);
    delay(speed);
    digitalWrite(pin, LOW);
    delay(speed);
  }
}0