# MCP2003B LIN AOB - Slave Example

Sketch: [`lin_slave_example.ino`](lin_slave_example.ino)

This example runs `InxMcp2003b` as a LIN slave at 10,000 baud. It waits for the
eight-byte command sent by the master example and reports whether the payload
matches.

## Connections

| Arduino Uno pin | LIN AOB signal | Purpose |
|-----------------|----------------|---------|
| D1 / TX | RX | LIN UART transmit |
| D0 / RX | TX | LIN UART receive |
| GND | GND | Common ground |

Use the board's wiring documentation for bus power and vehicle-bus connections.

## Debug output

The sketch uses `SoftwareSerial` at 115200 baud for debug output. Connect Uno
D11 (software-serial TX) to the USB-to-TTL adapter's RX pin and connect grounds.
Open the serial monitor or adapter terminal at 115200 baud.

Expected output after receiving the matching frame:

```text
Lin Responder Start
Received Data: 1, 2, 3, 4, 5, 6, 7, 8
Received Data Matched: OK
```
