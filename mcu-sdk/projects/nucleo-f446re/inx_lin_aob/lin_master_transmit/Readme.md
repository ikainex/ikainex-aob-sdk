# MCP2003B LIN Master - Nucleo-F446RE

## Example description

This project configures the MCP2003B LIN middleware as a master and transmits
one eight-byte frame with ID `0x17`. The LIN bus baud rate is 10000 and the
configured break width is 11 bits. Console output is sent on USART3 at
115200 baud.

The project uses the `inx_mcp2003b` STM32 middleware with USART2 for LIN
communication and PB6 to enable the LIN transceiver.

## Keywords

STM32F446RE, Nucleo-F446RE, MCP2003B, LIN, USART2, USART3

## Directory contents

- `lin_master_transmit.ioc` - STM32CubeMX peripheral and board configuration.
- `Inc/` - Application and STM32 HAL configuration headers.
- `Src/main.c` - LIN setup, frame data, transmit operation, and console output.
- `STM32CubeIDE/` - STM32CubeIDE project and linker configuration.
- `.mxproject` - STM32CubeIDE project metadata.

## Hardware and software environment

- NUCLEO-F446RE with an MCP2003B LIN AOB and a LIN bus peer.
- USART2 (PA2/PA3) is used for LIN communication at 10000 baud.
- PB6 controls the LIN transceiver's enable input.
- USART3 TX (PB10 / Arduino D6) carries console output at 115200 baud.
- The project uses STM32CubeF4 V1.28.3 and STM32CubeIDE.

This example has been tested with the STMicroelectronics NUCLEO-F446RE board
and can be tailored to other supported STM32F4 devices and development boards.

### LIN connections

| Nucleo-F446RE pin | LIN AOB pin | Signal |
|-------------------|-------------|--------|
| PA2 (Arduino D1) | RX | MCU TX to transceiver |
| PA3 (Arduino D0) | TX | Transceiver TX to MCU RX |
| PB6 | EN | Transceiver enable |
| GND | GND | Common ground |

### Console connection

Connect PB10 (Arduino D6, USART3 TX) to the RX pin of a USB-to-UART adapter
and connect the grounds. Open the adapter's serial port at 115200 baud.

On the Nucleo-F446RE, close solder bridges SB62 and SB63 when required to route
the USART2 signal to the Arduino header; check the board revision's user manual.

## How to build and run

1. Connect the LIN AOB to the Nucleo and ensure a LIN slave is available on the
   bus.
2. Open or import `STM32CubeIDE/.project` in STM32CubeIDE.
3. Connect the Nucleo board over ST-LINK USB, build the project, and flash it.
4. Open the USART3 console at 115200 baud.

The console reports `LIN Master Start` and the transmitted ID, payload, and
configured bus baud rate.
