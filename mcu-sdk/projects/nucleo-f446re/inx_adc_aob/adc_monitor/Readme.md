# MCP3204 ADC Monitor - Nucleo-F446RE

## Example description

This project reads all four single-ended channels of the MCP3204 ADC over SPI1.
It prints each 12-bit raw reading and its calculated voltage in millivolts once
per second. The configured reference voltage is 4096 mV.

The application uses the `inx_mcp3204` STM32 middleware and sends its console
output through USART2 at 115200 baud.

## Keywords

STM32F446RE, Nucleo-F446RE, MCP3204, ADC, SPI1, USART2

## Directory contents

- `adc_monitor.ioc` - STM32CubeMX peripheral and board configuration.
- `Inc/` - Application and STM32 HAL configuration headers.
- `Src/main.c` - ADC setup, channel reads, voltage conversion, and console output.
- `STM32CubeIDE/` - STM32CubeIDE project and linker configuration.
- `.mxproject` - STM32CubeIDE project metadata.

## Hardware and software environment

- NUCLEO-F446RE with an MCP3204 ADC AOB.
- SPI1 is used for ADC communication; the middleware is configured for a
  2 MHz SPI clock.
- USART2 sends console output at 115200 baud. On the Nucleo board, use the
  ST-LINK virtual COM port.
- The project uses STM32CubeF4 V1.28.3 and STM32CubeIDE.

This example has been tested with the STMicroelectronics NUCLEO-F446RE board
and can be tailored to other supported STM32F4 devices and development boards.

### SPI connections

| Nucleo-F446RE pin | MCP3204 AOB pin | Signal |
|-------------------|-----------------|--------|
| PB6 (Arduino D10) | CS | Chip select |
| PA5 (Arduino D13) | SCK | SPI clock |
| PA7 (Arduino D11) | SDI | MCU MOSI to ADC |
| PA6 (Arduino D12) | SDO | ADC MISO to MCU |
| GND | GND | Common ground |

Connect the AOB power and reference inputs according to its hardware
documentation. The configured 4096 mV reference must match the hardware.

## How to build and run

1. Open or import `STM32CubeIDE/.project` in STM32CubeIDE.
2. Connect the Nucleo board over ST-LINK USB, build the project, and flash it.
3. Open the board's ST-LINK virtual COM port at 115200 baud.

The console reports `ADC Monitor Start`, followed by the raw value and voltage
for channels CH0 through CH3. The display is refreshed every second.
