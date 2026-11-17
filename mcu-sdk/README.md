# MCU SDK

The MCU SDK contains STM32 middleware drivers, STM32CubeIDE projects, shared
utilities, and the STM32Cube/CMSIS packages used by the projects.

The current ADC and LIN middleware drivers target STM32F4 and use the STM32 HAL
directly. The repository also contains an STM32L5 Cortex-M33 CAN FD echo project split
into Secure and NonSecure applications. An STM32L5 CAN FD middleware driver is
not currently present.

## Repository structure

```text
mcu-sdk/
├── doc/                         Toolchain integration guides
├── drivers/
│   ├── STM32Cube_FW_F4_V1.28.3/ STM32F4 Cube and CMSIS files
│   └── STM32Cube_FW_L5_V1.6.0/ STM32L5 Cube and CMSIS files
├── middlewares/
│   └── stm32/
│       ├── inx_adc/             MCP3204 driver
│       └── inx_lin/             MCP2003B driver
├── projects/
│   ├── nucleo-f446re/
│   │   ├── inx_adc_aob/adc_monitor/
│   │   └── inx_lin_aob/lin_master_transmit/
│   └── nucleo-l552ze-q/
│       └── inx_canfd_aob/canfd_echo/
├── scripts/                     Setup, distribution, and board config files
└── utility/                     Shared utilities
```

## Middleware APIs

The MCP3204 ADC C API is declared in
[`middlewares/stm32/inx_adc/inx_mcp3204.h`](middlewares/stm32/inx_adc/inx_mcp3204.h).
It provides `inx_mcp3204_open`, `inx_mcp3204_init`, and read functions for raw,
single-ended voltage, and differential voltage values.

The MCP2003B LIN C API is declared in
[`middlewares/stm32/inx_lin/inx_mcp2003b.h`](middlewares/stm32/inx_lin/inx_mcp2003b.h).
It provides configuration, start/stop, message transmit, and message read
functions.

Both middleware drivers accept STM32 HAL peripheral handles through their
hardware descriptors. Applications are responsible for initializing clocks,
GPIO, SPI, and UART before using the drivers.

## Sample projects

The sample projects are under `projects/` and include STM32CubeIDE project
metadata and `.ioc` configuration files. Open the corresponding project with
STM32CubeIDE to build and flash it:

- Nucleo-F446RE ADC monitor:
  [`projects/nucleo-f446re/inx_adc_aob/adc_monitor/`](projects/nucleo-f446re/inx_adc_aob/adc_monitor/)
  ([project README](projects/nucleo-f446re/inx_adc_aob/adc_monitor/Readme.md))
- Nucleo-F446RE LIN master:
  [`projects/nucleo-f446re/inx_lin_aob/lin_master_transmit/`](projects/nucleo-f446re/inx_lin_aob/lin_master_transmit/)
  ([project README](projects/nucleo-f446re/inx_lin_aob/lin_master_transmit/Readme.md))
- Nucleo-L552ZE-Q CANFD echo:
  [`projects/nucleo-l552ze-q/inx_canfd_aob/canfd_echo/`](projects/nucleo-l552ze-q/inx_canfd_aob/canfd_echo/)
  ([project README](projects/nucleo-l552ze-q/inx_canfd_aob/canfd_echo/Readme.md))

The CANFD project is a target-specific sample and does not imply a portable
CANFD middleware API is available.

## Toolchain and setup

Integration guides for CMake, IAR, and Keil are available in [`doc/`](doc/).
The `scripts/` directory contains setup scripts for Windows and Unix-like
systems, board configuration files, and the distribution script.

## License

Third-party license files are included with the vendored packages under
`drivers/`. This checkout does not include a repository-level SDK license file.
