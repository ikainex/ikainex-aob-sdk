# MCP2003B LIN AOB - Arduino Library

Arduino library for LIN master and slave communication using the MCP2003B LIN
transceiver. The library uses an Arduino `HardwareSerial` instance for bus
communication and a GPIO pin to generate the LIN break signal.

## Features

- Master and slave modes.
- Protected identifier generation and validation.
- Classic and enhanced checksum modes.
- Message status and timeout reporting.

## Hardware

Connect the LIN AOB to an Arduino hardware UART and the LIN bus. The example
sketches use Uno pins D0/RX and D1/TX. The constructor also takes the RX and TX
pin numbers so the driver can control the break signal; use the pin mapping
appropriate to your board and transceiver.

The LIN bus requires a master and at least one slave. Ensure the transceiver,
bus supply, and ground are wired according to the board documentation.

## Installation

Copy the `inx_lin_aob` folder into the Arduino sketchbook's `libraries` folder,
then restart the IDE. The library is named **LIN Add-on Board** in its metadata.

## Quick start

### Master

```cpp
#include <InxMcp2003b.h>

InxMcp2003b lin(Serial, 1, 0, 13);

void setup() {
    if (lin.beginMaster(10000) != INX_MCP2003B_STATUS_OK) {
        while (true) {}
    }

    lin.setEnhancedChecksum(false);
}

void loop() {
    const uint8_t data[] = {0x01, 0x02};
    lin.writeMessage(0x10, data, sizeof(data));

    if (lin.requestMessage(0x20) == INX_MCP2003B_STATUS_OK) {
        Inx_Mcp2003b_Msg msg;
        if (lin.readMessageWait(&msg, 2, 100) == INX_MCP2003B_STATUS_OK) {
            // Handle the response.
        }
    }

    delay(100);
}
```

### Slave

```cpp
#include <InxMcp2003b.h>

InxMcp2003b lin(Serial, 1, 0, 13);

void setup() {
    lin.beginSlave(10000);
    lin.setEnhancedChecksum(false);
}

void loop() {
    Inx_Mcp2003b_Msg msg;
    if (lin.readMessageWait(&msg, 8, 1000) == INX_MCP2003B_STATUS_OK) {
        const uint8_t response[] = {0xAA, 0xBB};
        lin.sendResponse(response, sizeof(response));
    }
}
```

## API reference

### Construction and lifecycle

```cpp
InxMcp2003b(HardwareSerial &serial,
            uint8_t tx_pin,
            uint8_t rx_pin,
            uint8_t break_width = 13);
uint16_t beginMaster(uint32_t baudrate);
uint16_t beginSlave(uint32_t baudrate);
void end();
```

### Message operations

```cpp
uint16_t writeMessage(uint8_t id, const uint8_t *data, uint8_t length);
uint16_t requestMessage(uint8_t id);
uint16_t readMessageWait(
    Inx_Mcp2003b_Msg *msg, uint8_t dlc, uint32_t timeout_ms);
uint16_t sendResponse(const uint8_t *data, uint8_t length);
```

The message structure `Inx_Mcp2003b_Msg` contains the frame ID, up to eight
payload bytes, payload length, checksum, and status flags. `dlc` supplies the
expected payload length when waiting for a message.

### Configuration and status

```cpp
void setMode(bool is_master);
void setEnhancedChecksum(bool enable);
bool isMaster() const;
uint16_t getLastStatus() const;
void flushBuffers();
int available() const;
```

Status codes and mode flags use the `INX_MCP2003B_` prefix, including
`INX_MCP2003B_STATUS_OK`, `INX_MCP2003B_STATUS_TIMEOUT`,
`INX_MCP2003B_MASTER`, and `INX_MCP2003B_SLAVE`.

## Examples

- [LIN master transmit](examples/lin_master_transmit/lin_master_transmit.ino)
- [LIN slave](examples/lin_slave_example/lin_slave_example.ino)

The example folders also contain wiring and output notes.

## MCU SDK C API

The MCU SDK exposes a separate C API in
[`mcu-sdk/middlewares/stm32/inx_lin/inx_mcp2003b.h`](../../mcu-sdk/middlewares/stm32/inx_lin/inx_mcp2003b.h).
It provides functions such as `inx_mcp2003b_open`, `inx_mcp2003b_config`,
`inx_mcp2003b_start`, `inx_mcp2003b_write_message`, and
`inx_mcp2003b_read_message_wait`. The MCU API uses STM32 HAL UART handles;
this library uses Arduino `HardwareSerial`.

## Compatibility and license

The LIN interface requires an Arduino core that provides `HardwareSerial`.
This library is part of the Ikainex Add-on Board SDK.
