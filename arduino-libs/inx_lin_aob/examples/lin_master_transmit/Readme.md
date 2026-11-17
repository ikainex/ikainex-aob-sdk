# MCP2003B LIN AOB - Master Example

Sketch: [`lin_master_transmit.ino`](lin_master_transmit.ino)

This example runs `InxMcp2003b` as a LIN master at 10,000 baud and transmits
frame ID `0x17` with an eight-byte payload. It uses the hardware UART on the Uno
for LIN communication.

## Connections

| Arduino Uno pin | LIN AOB signal | Purpose |
|-----------------|----------------|---------|
| D1 / TX | TX | LIN UART transmit |
| D0 / RX | RX | LIN UART receive |
| GND | GND | Common ground |

Use the board's wiring documentation for bus power and vehicle-bus connections.

## Debug output

The sketch uses `SoftwareSerial` at 115200 baud for debug output. Connect Uno
D11 (software-serial TX) to the USB-to-TTL adapter's RX pin and connect grounds.
Open the serial monitor or adapter terminal at 115200 baud.

Expected output:

```text
Lin Commander Start
Sent Data: 1, 2, 3, 4, 5, 6, 7, 8
Lin write message Success
```
