// Tested : ✅
// Works  : ✅



#include <SoftwareSerial.h>
#include <SD.h>

#define RFID_RX 2
#define RFID_TX 3
#define SD_CS 10

#define GREEN_LED 4
#define RED_LED 5

const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;

SoftwareSerial ssrfid(RFID_RX, RFID_TX);
uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

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

  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("⚠️ Chyba: SD karta sa nenačítala.");
    digitalWrite(RED_LED, HIGH);
    while (true);
  }

  sd_ready = true;
  Serial.println("✅ SD karta inicializovaná.");

  // Overenie existencie súboru
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
      Serial.println("⚠️ Chyba: Nepodarilo sa vytvoriť súbor.");
      digitalWrite(RED_LED, HIGH);
      while (true);
    }
  }

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (ssrfid.available() > 0) {
    digitalWrite(GREEN_LED, HIGH);  // Indikuj čítanie

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
}

void process_tag() {
  char* msg_data_tag = (char*)(buffer + 3);

  Serial.println("-------------------");
  Serial.print("UID: ");
  for (int i = 0; i < DATA_TAG_SIZE; ++i) {
    Serial.print(msg_data_tag[i]);
  }

  // Detekcia typu podľa začiatku UID
  String card_type = "";
  if (msg_data_tag[0] == '0' && msg_data_tag[1] == '0') {
    card_type = "EM4100";
  } else {
    card_type = "Unknown";
  }

  Serial.print(" , Card Type: ");
  Serial.println(card_type);
  Serial.println("-------------------");

  // Pokus o zápis do súboru, ktorý bol overený/skontrolovaný v setup()
  if (sd_ready) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.print("UID: ");
      for (int i = 0; i < DATA_TAG_SIZE; ++i) {
        file.print(msg_data_tag[i]);
      }
      file.print(" , Card Type: ");
      file.println(card_type);
      file.close();

      // Indikácia úspechu
      blinkLED(GREEN_LED, 3, 100);
    } else {
      Serial.println("⚠️ Chyba: Nepodarilo sa otvoriť existujúci súbor.");
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
