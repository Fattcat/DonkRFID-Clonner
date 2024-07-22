# DonkRFID-Clonner

## 📝Necessary modules and stuff
1. Arduino Nano
2. Oled Display 0.96 Inch
3. RC522 RFID Module
4. 2 Buttons
5. 2x Resistor with 220 Ohm (for buttons !)
6. Bread Board
7. Some Wires (about 20pcs)
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
- IRQ -> N/A
- GND -> GND
- RST -> D9
- 3.3V -> 3.3V

## OLED display 🖥:
- VCC -> 5V
- GND -> GND
- SDA -> A4
- SCL -> A5

## 🖲 Tlačidlá:
- Tlačidlo Read -> D2
- Tlačidlo Emulate -> D3

## 💡LED diódy:
- GREEN LED -> D5 (cez 220 OHM rezistor)
- RED LED -> D6 (cez 220 OHM rezistor)
