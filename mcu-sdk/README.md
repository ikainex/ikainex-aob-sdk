# mcu-sdk

C SDK for Ikainex add-on boards, with STM32F4 as its current supported target.
Each board driver is a small wrapper around the STM32Cube
HAL: applications use the board API while passing their native STM32 HAL
peripheral handles.

## Design

```
Application ──▶ include/<board>_board.h ──▶ drivers/stm/<board>/
                                                    │
                                                    ▼
                                            STM32Cube HAL
```

- **`include/`** — public board APIs. Their hardware descriptors accept
  STM32Cube HAL handles such as `SPI_HandleTypeDef` and
  `UART_HandleTypeDef`.
- **`drivers/stm/`** — STM32 board wrappers and protocol logic. These call the
  STM32Cube HAL directly; there is no intermediate platform-dependent HAL.
- **`samples/`** — STM32CubeIDE projects showing how applications initialize
  peripherals and pass their handles to the board wrappers.
- **`scripts/`** — release and build tooling, including per-board version and
  configuration files.
- **`doc/`** — toolchain integration guides (Keil, IAR, CMake).

The STM32Cube HAL and CMSIS device files are supplied by the target
STM32CubeIDE project or toolchain and are not vendored in this SDK. Configure
peripheral clocks and pin muxing in the application as usual before opening a
board wrapper. Closing a wrapper releases its SDK-owned state; it does not
deinitialize the application-owned STM32 peripheral handle.

## Directory structure

```
mcu-sdk
├── doc/                          Integration guides (Keil / IAR / CMake)
├── scripts/                      Release and build tooling
├── include/                      Public board APIs
├── drivers/
│   └── stm/                      STM32 board wrappers
├── samples/                      STM32CubeIDE examples
└── utility/                      Shared helpers
```

## Supported targets

| Vendor | Family / Board    | Status         |
|--------|-------------------|----------------|
| STM32  | Nucleo-F446RE     | Primary target |

## Using a board wrapper

Initialize the required STM32 peripheral using STM32Cube HAL, then pass the
handle in the board hardware descriptor. For example, the ADC board wrapper
accepts an initialized SPI handle and a GPIO port/pin for chip select; the LIN
board wrapper accepts a UART handle.

Build the relevant source under `drivers/stm/` with the STM32Cube HAL sources
and include directories for the target MCU. Do not add a separate SDK HAL
adapter layer.

## Contributing

Add a public header under `include/`, an STM32 implementation under
`drivers/stm/<module>/`, a configuration and version file under
`scripts/boards/`, and a target sample under `samples/`.

## License

_TODO: Add License information_
