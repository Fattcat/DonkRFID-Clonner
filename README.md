<div align="center">
   <h1>DonkRFID-Clonner</h1>
   <p>With this you are able to clone your RFID tag</p>
</div>
<div align="center">
  <h1>📝Necessary modules and stuff</h1>
</div>

1. Arduino Nano
2. Oled Display 0.96 Inch
3. RC522 RFID Module
4. 2 Buttons
5. 2x Resistor with 220 Ohm (for buttons !)
6. Bread Board
7. Some Wires (about 20pcs)
8. Arduino SD Card Module (Used only for DonkRFID-OledV3-SaveToSD-Card.ino) 
### Optional for more precisely use.
1. 2x LED Diodes (RED and GREEN) and 2x 220 Ohm resistor (if u use LEDs)
### 📣 INFO 📣
- Just added for show status (RED lights up if emulating or reading failed and green for opposite)
# Connection

## MFRC522 RFID:
- SDA -> D10
- SCK -> D13
- MOSI -> D11
- MISO -> D12
- IRQ -> Not connected
- GND -> GND
- RST -> D9
- 3.3V -> 3.3V

## OLED display 🖥:
- VCC -> 5V
- GND -> GND
- SDA -> A4
- SCL -> A5

## SD Card:
- VCC -> 5V
- GND -> GND
- MISO -> D12
- MOSI -> D11
- SCK -> D13
- CS -> D4

## 🖲 Buttons :
- Button Read -> D2
- Button Emulate -> D3

## 💡LED diodes:
- GREEN LED -> D5 (throught 220 OHM resistor)
- RED LED -> D6 (throught 220 OHM resistor)
