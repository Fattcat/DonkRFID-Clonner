#include <SoftwareSerial.h>
#include <SD.h>

#define RFID_RX 6
#define RFID_TX 8
#define SD_CS 10 // Chip Select pin pre SD kartu

const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;

SoftwareSerial ssrfid(RFID_RX, RFID_TX);
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

void setup() {
  Serial.begin(9600);
  ssrfid.begin(9600);
  ssrfid.listen();

  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("⚠️ Chyba: SD karta sa nenačítala.");
    while (true);
  }
  Serial.println("✅ SD karta inicializovaná.");
  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (ssrfid.available() > 0) {
    bool call_extract_tag = false;
    int ssvalue = ssrfid.read();

    if (ssvalue == -1) return;

    if (ssvalue == 2) buffer_index = 0;
    else if (ssvalue == 3) call_extract_tag = true;

    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("⚠️ Chyba: Buffer overflow!");
      return;
    }

    buffer[buffer_index++] = ssvalue;

    if (call_extract_tag && buffer_index == BUFFER_SIZE) {
      process_tag();
    }
  }
}

void process_tag() {
  char* msg_data_tag = (char*)(buffer + 3); // offset o 3 pre prístup k samotnému TAGu

  Serial.println("-------------------");
  Serial.print("ID karty (HEX): ");
  for (int i = 0; i < DATA_TAG_SIZE; ++i) {
    Serial.print(msg_data_tag[i]);
  }
  Serial.println();
  Serial.println("-------------------");

  // Uloženie na SD kartu
  File file = SD.open("125khz-data.txt", FILE_WRITE);
  if (file) {
    for (int i = 0; i < DATA_TAG_SIZE; ++i) {
      file.print(msg_data_tag[i]);
    }
    file.println();
    file.println("-------------------");
    file.close();
  } else {
    Serial.println("⚠️ Chyba: Nepodarilo sa otvoriť súbor.");
  }

  buffer_index = 0;
}
