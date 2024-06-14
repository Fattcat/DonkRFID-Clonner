# DonkRFID-Clonner

## Necessary modules and stuff
1. Arduino Nano
2. RC522 RFID Module
3. 2 Buttons
4. 2x LED Diodes (RED and GREEN)
5. 2x Resistor with 220 Ohm
6. Bread Board
7. Some Wires (about 20pcs)

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

## OLED displej:
- VCC -> 5V
- GND -> GND
- SDA -> A4
- SCL -> A5

## Tlačidlá:
- Tlačidlo UP -> D2
- Tlačidlo DOWN -> D3
- Tlačidlo ENTER -> D4

## LED diódy:
- GREEN LED -> D5 (cez 220 OHM rezistor)
- RED LED -> D6 (cez 220 OHM rezistor)
