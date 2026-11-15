# MCP3204 ADC AOB - Arduino Library

Arduino library for reading single-ended and differential analog inputs through
the MCP3204 12-bit ADC over SPI.

## Features

- Uses the built-in Arduino SPI library.
- Reads four single-ended channels or the supported differential pairs.
- Converts ADC readings to millivolts using the configured reference voltage.

## Hardware

Connect the ADC AOB to an Arduino board with SPI support. SPI pin assignments
depend on the Arduino model; on an Uno they are MOSI 11, MISO 12, and SCK 13.
The chip-select pin is configurable (the examples use pin 10). Set the reference
voltage to match the board's hardware.

## Installation

Copy the `inx_adc_aob` folder into the Arduino sketchbook's `libraries` folder,
then restart the IDE. The library is named **ADC Add-on Board** in its metadata.

## Quick start

```cpp
#include <InxMcp3204.h>

InxMcp3204 adc(10, INX_MCP3204_VREF_5000MV, 2000000);

void setup() {
    Serial.begin(115200);
    if (adc.begin() != INX_STATUS_OK) {
        Serial.println("ADC initialization failed");
        while (true) {}
    }
}

void loop() {
    uint16_t raw_value = 0;
    uint16_t voltage_mv = 0;

    if (adc.readRaw(INX_MCP3204_CH0, &raw_value) == INX_STATUS_OK &&
        adc.readVoltage(INX_MCP3204_CH0, &voltage_mv) == INX_STATUS_OK) {
        Serial.print("Raw: ");
        Serial.print(raw_value);
        Serial.print(", voltage: ");
        Serial.print(voltage_mv);
        Serial.println(" mV");
    }

    delay(1000);
}
```

## API reference

### Construction and lifecycle

```cpp
InxMcp3204(uint8_t cs_pin,
           uint16_t vref_mv = INX_MCP3204_VREF_5000MV,
           uint32_t spi_freq = 2000000);
Inx_Mcp3204_Status begin();
void end();
```

`begin()` initializes SPI and the chip-select pin. `end()` deinitializes the
library interface.

### Read operations

```cpp
Inx_Mcp3204_Status readRaw(Inx_Mcp3204_Ch channel, uint16_t *raw_value);
Inx_Mcp3204_Status readVoltage(Inx_Mcp3204_Ch channel, uint16_t *voltage_mv);
Inx_Mcp3204_Status readDifferentialVoltage(
    Inx_Mcp3204_DiffCh channel, uint16_t *voltage_mv);
```

Single-ended channels are `INX_MCP3204_CH0` through `INX_MCP3204_CH3`.
Differential channels are `INX_MCP3204_DIFF_CH01`, `INX_MCP3204_DIFF_CH10`,
`INX_MCP3204_DIFF_CH23`, and `INX_MCP3204_DIFF_CH32`.

The status type is `Inx_Mcp3204_Status`; its values are `INX_STATUS_OK`,
`INX_STATUS_ERR`, `INX_STATUS_INVALID_CH`, and `INX_STATUS_INVALID_PARAM`.
Reference voltage constants are `INX_MCP3204_VREF_3300MV`,
`INX_MCP3204_VREF_4096MV`, and `INX_MCP3204_VREF_5000MV`.

### SPI and reference voltage

```cpp
void setSPIFrequency(uint32_t freq);
uint16_t getVref() const;
```

See [`examples/adc_monitor/adc_monitor.ino`](examples/adc_monitor/adc_monitor.ino)
for a complete single-ended and differential reading example.

## MCU SDK C API

The MCU SDK exposes a separate C API in
[`mcu-sdk/middlewares/stm32/inx_adc/inx_mcp3204.h`](../../mcu-sdk/middlewares/stm32/inx_adc/inx_mcp3204.h).
Its functions include `inx_mcp3204_open`, `inx_mcp3204_init`,
`inx_mcp3204_read_raw`, `inx_mcp3204_read_mv`, and
`inx_mcp3204_read_differential_mv`. The Arduino class wraps the same ADC
functionality with Arduino SPI and pin APIs; the two APIs have different
initialization and status types.

## Compatibility

The library uses Arduino SPI and targets boards that provide the standard SPI
library. Check your board's SPI pin mapping and supported Arduino core.

## License

This library is part of the Ikainex Add-on Board SDK.
