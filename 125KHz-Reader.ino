#include <SoftwareSerial.h>

#define RDM6300_RX 2  // Pripojené na TX pinu RDM6300
#define RDM6300_TX 3  // Nevyužité, ale potrebné pre SoftSerial

SoftwareSerial rfidSerial(RDM6300_RX, RDM6300_TX); // RX, TX

void setup() {
  Serial.begin(9600);
  rfidSerial.begin(9600);

  Serial.println("Čakám na RFID kartu...");
}

void loop() {
  if (rfidSerial.available() >= 14) { // 14 bajtov: STX + 10 dát + 2 checksum + ETX
    if (rfidSerial.read() == 0x02) { // Začiatok dátového rámca

      char tagData[11]; // 10 znakov + '\0'
      for (int i = 0; i < 10; i++) {
        while (!rfidSerial.available());
        tagData[i] = rfidSerial.read();
      }
      tagData[10] = '\0';

      char checksum[3]; // 2 znaky + '\0'
      for (int i = 0; i < 2; i++) {
        while (!rfidSerial.available());
        checksum[i] = rfidSerial.read();
      }
      checksum[2] = '\0';

      // Očakávaj ukončovací bajt (0x03)
      while (!rfidSerial.available());
      if (rfidSerial.read() != 0x03) {
        Serial.println("Chyba: Nesprávny end byte");
        return;
      }

      // Výpis do Serial monitora
      Serial.print("ID karty: ");
      Serial.println(tagData);
      Serial.print("Checksum: ");
      Serial.println(checksum);
      Serial.println("-------------------");

      delay(2000); // Anti-duplikácia čítania tej istej karty
    }
  }
}
