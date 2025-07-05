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
| Modul / Funkcia       | Signál / Popis       | Pin Arduino Nano    |
| --------------------- | -------------------- | ------------------- |
| **MFRC522 (SPI)**     | SDA (SS)             | **D10**             |
|                       | SCK                  | D13 (SPI SCK)       |
|                       | MOSI                 | D11 (SPI MOSI)      |
|                       | MISO                 | D12 (SPI MISO)      |
|                       | RST                  | **D9**              |
|                       | VCC                  | 3.3V                |
|                       | GND                  | GND                 |
| **SD Karta (SPI)**    | CS                   | **D4**              |
|                       | SCK                  | D13 (zdieľané)      |
|                       | MOSI                 | D11 (zdieľané)      |
|                       | MISO                 | D12 (zdieľané)      |
|                       | VCC                  | 5V                  |
|                       | GND                  | GND                 |
| **OLED I2C**          | SDA                  | **A4**              |
|                       | SCL                  | **A5**              |
|                       | VCC                  | 3.3V / 5V           |
|                       | GND                  | GND                 |
| **RDM6300 (125 kHz)** | TX (výstup z modulu) | **D6** (RX Arduino) |
|                       | VCC                  | 5V                  |
|                       | GND                  | GND                 |

# UPDATES
  - Will be added 125 KHz support
  - Auto Detect Mode
  - Auto Saving data separately to SD Card

# ISSUES
  - if you have any problems, doublecheck you wiring & functional modules like RFID reader, display, etc...
  - Make sure you have installed drivers (arduino clones must have driver CH340) [Download Here](https://www.wch.cn/downloads/CH341SER_EXE.html)
