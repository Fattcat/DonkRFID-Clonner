#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define RDM6300_RX 2  // Pripojené na TX pinu RDM6300
#define RDM6300_TX 3  // Nevyužité, ale potrebné pre SoftSerial
#define SD_CS 10      // Chip select pre SD kartu

SoftwareSerial rfidSerial(RDM6300_RX, RDM6300_TX); // RX, TX
File dataFile;

void setup() {
  Serial.begin(9600);
  rfidSerial.begin(9600);
  
  Serial.println("Inicializujem SD kartu...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD karta sa nenašla alebo chyba inicializácie!");
    return;
  }
  Serial.println("SD karta je pripravená.");

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (rfidSerial.available() >= 14) { // 1 byte start, 10 data, 2 checksum, 1 end
    if (rfidSerial.read() == 0x02) { // Start byte

      char tagData[11]; // 10 znakov + '\0'
      for (int i = 0; i < 10; i++) {
        while (!rfidSerial.available()); // Čakaj na dáta
        tagData[i] = rfidSerial.read();
      }
      tagData[10] = '\0'; // Ukončovací znak pre string

      // Prečítaj checksum
      char checksum[3];
      for (int i = 0; i < 2; i++) {
        while (!rfidSerial.available());
        checksum[i] = rfidSerial.read();
      }
      checksum[2] = '\0';

      // Prečítaj end byte
      while (!rfidSerial.available());
      if (rfidSerial.read() != 0x03) {
        Serial.println("Chyba: Nesprávny end byte");
        return;
      }

      // Výpis dát do Serial Monitoru
      Serial.print("ID karty: ");
      Serial.println(tagData);
      Serial.print("Checksum: ");
      Serial.println(checksum);
      Serial.println("-------------------");

      // Zápis do SD karty
      dataFile = SD.open("125khz-data.txt", FILE_WRITE);
      if (dataFile) {
        dataFile.print("ID karty: ");
        dataFile.println(tagData);
        dataFile.print("Checksum: ");
        dataFile.println(checksum);
        dataFile.println("-------------------");
        dataFile.close();
      } else {
        Serial.println("Chyba pri zápise do súboru!");
      }

      delay(2000); // Zamedzenie opakovanému čítaniu tej istej karty
    }
  }
}
