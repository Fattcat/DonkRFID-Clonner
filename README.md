<div align="center">
   <h1>DonkRFID-Clonner</h1>
   <p>With this you are able to clone your RFID Card or Tag</p>
   <p>please support this repository with STAR</p>

   <!-- Badges -->
   <a href="https://github.com/Fattcat/DonkRFID-Clonner" title="Go to GitHub repo"><img src="https://img.shields.io/static/v1?label=Fattcat&message=DonkRFID-Clonner&color=white&logo=github" alt="Fattcat - DonkRFID-Clonner"></a>
   <a href="https://github.com/Fattcat/DonkRFID-Clonner"><img src="https://img.shields.io/github/stars/Fattcat/DonkRFID-Clonner?style=social" alt="stars - DonkRFID-Clonner"></a>
   <a href="https://github.com/Fattcat/DonkRFID-Clonner"><img src="https://img.shields.io/github/forks/Fattcat/DonkRFID-Clonner?style=social" alt="forks - DonkRFID-Clonner"></a>
   
   <p>Checkout Stable Release Versions</p>
</div>

<div align="center">
  <h1>📝Necessary modules and stuff</h1>
</div>

- Official CH340 Driver [Download Here](https://www.wch.cn/downloads/CH341SER_EXE.html) (ONLY FOR Arduino CLONE)
- Arduino Nano
- Oled Display 0.96 Inch
- RC522 RFID Module
- 2 Buttons
- 2x Resistor with 220 Ohm (for buttons !)
- Bread Board
- Some Wires (about 20pcs)
- Arduino SD Card Module (Used only for DonkRFID-OledV3-SaveToSD-Card.ino) 
### Optional for more precisely use.
- 2x LED Diodes (RED and GREEN) and 2x 220 Ohm resistor (if u use LEDs)
### 📣 INFO 📣
- Just added for show status (RED lights up if emulating or reading failed and green for opposite)

## Table of connection
| Komponent              | component | Arduino pin | Popis                                              |
| ---------------------- | --------- | ----------- | -------------------------------------------------- |
| **OLED displej (I2C)** | SDA       | A4          | I2C dátová linka                                   |
|                        | SCL       | A5          | I2C hodinová linka                                 |
| **RDM6300 (125 kHz)**  | TX        | D6 (RX)     | Prijímanie dát z RFID čítačky cez `SoftwareSerial` |
|                        | RX        | D7 (TX)     | Nepoužívané – môže ostať nezapojené                |
| **RC522 (13.56 MHz)**  | RST       | D9          | Reset RFID čítačky                                 |
|                        | SS (SDA)  | D10         | SPI chip select pre RC522                          |
|                        | MOSI      | D11         | SPI MOSI (zdieľané so SD kartou)                   |
|                        | MISO      | D12         | SPI MISO (zdieľané so SD kartou)                   |
|                        | SCK       | D13         | SPI hodiny (zdieľané so SD kartou)                 |
| **SD karta (SPI)**     | CS        | D4          | SPI chip select pre SD modul                       |
|                        | MOSI      | D11         | SPI MOSI (zdieľané s RC522)                        |
|                        | MISO      | D12         | SPI MISO (zdieľané s RC522)                        |
|                        | SCK       | D13         | SPI hodiny (zdieľané s RC522)                      |
| **LED diódy**          | Zelená    | D8          | Indikácia úspechu (napr. uloženie UID)             |
|                        | Červená   | D5          | Indikácia chyby (napr. chyba zápisu na SD)         |
| **Tlačidlá**           | READ mód  | D2          | Čítanie UID (aktívne pri stlačení, `INPUT_PULLUP`) |
|                        | WRITE mód | D3          | Zápis UID na SD kartu (`INPUT_PULLUP`)             |


# UPDATES
  - Will be added 125 KHz support
  - Auto Detect Mode
  - Auto Saving data separately to SD Card

# ISSUES
  - if you have any problems, doublecheck you wiring & functional modules like RFID reader, display, etc...
  - Make sure you have installed drivers (arduino clones must have driver CH340) [Download Here](https://www.wch.cn/downloads/CH341SER_EXE.html)
