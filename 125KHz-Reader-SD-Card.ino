#include <SoftwareSerial.h>
#include <SD.h>

#define RFID_RX 6
#define RFID_TX 8
#define SD_CS 10

#define GREEN_LED 3
#define RED_LED 4

const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;

SoftwareSerial ssrfid(RFID_RX, RFID_TX);
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

bool sd_initialized = false;

void setup() {
  Serial.begin(9600);
  ssrfid.begin(9600);
  ssrfid.listen();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("⚠️ Chyba: SD karta sa nenačítala.");
    digitalWrite(RED_LED, HIGH);  // SD karta nie je dostupná - trvalé svietenie
  } else {
    sd_initialized = true;
    Serial.println("✅ SD karta inicializovaná.");
  }

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (ssrfid.available() > 0) {
    digitalWrite(GREEN_LED, HIGH); // Začiatok čítania

    bool call_extract_tag = false;
    int ssvalue = ssrfid.read();
    if (ssvalue == -1) return;

    if (ssvalue == 2) buffer_index = 0;
    else if (ssvalue == 3) call_extract_tag = true;

    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("⚠️ Chyba: Buffer overflow!");
      blinkLed(RED_LED, 2, 500);
      digitalWrite(GREEN_LED, LOW);
      buffer_index = 0;
      return;
    }

    buffer[buffer_index++] = ssvalue;

    if (call_extract_tag && buffer_index == BUFFER_SIZE) {
      digitalWrite(GREEN_LED, LOW); // Dokončenie čítania
      process_tag();
    } else if (call_extract_tag) {
      Serial.println("⚠️ Chyba: Neúplný tag!");
      digitalWrite(GREEN_LED, LOW);
      blinkLed(RED_LED, 2, 500);
      buffer_index = 0;
    }
  }
}

void process_tag() {
  char* msg_data_tag = (char*)(buffer + 3); // prístup k UID
  char tag_str[DATA_TAG_SIZE + 1] = {0};
  strncpy(tag_str, msg_data_tag, DATA_TAG_SIZE);

  Serial.println("-------------------");
  Serial.print("ID karty (HEX): ");
  Serial.println(tag_str);
  Serial.println("-------------------");

  if (!sd_initialized) {
    Serial.println("⚠️ SD karta nie je inicializovaná!");
    blinkLed(RED_LED, 5, 150);
    return;
  }

  File file = SD.open("RFID-Data.txt", FILE_WRITE);
  if (file) {
    file.print("UID: ");
    file.print(tag_str);
    file.print(" , Card type: EM4100"); // Predvolený typ
    file.println();
    file.println(); // nový prázdny riadok
    file.close();

    blinkLed(GREEN_LED, 5, 100); // úspešný zápis
  } else {
    Serial.println("⚠️ Chyba: Nepodarilo sa otvoriť súbor!");
    blinkLed(RED_LED, 5, 150); // chyba zápisu
  }
  buffer_index = 0;
}

void blinkLed(int ledPin, int times, int duration_ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration_ms);
    digitalWrite(ledPin, LOW);
    delay(duration_ms);
  }
}
