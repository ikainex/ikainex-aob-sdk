# ikainex-aob-sdk - Ikainex Add-on Board SDK

SDK and examples for **Ikainex Add-on Boards**. This repository contains:
- **MCU SDK** — STM32 middleware and Nucleo sample projects
- **Arduino Libraries** — Ready-to-use Arduino libraries with examples
- **MicroPython Support** — Python integration for Inx boards

## Quick Links

- 📖 **[MCU SDK Documentation](mcu-sdk/README.md)** — C/C++ SDK details
- 🔧 **[Arduino Libraries](arduino-libs/Readme.md)** — Arduino integration guide
- 🐍 **[MicroPython Support](micropython/Readme.md)** — MicroPython module guide

## Project Structure

```
ikainex-aob-sdk/
├── mcu-sdk/              # STM32 SDK, middleware, and sample projects
│   ├── doc/              # Toolchain integration guides
│   ├── drivers/          # Vendored STM32Cube F4 and L5 packages
│   ├── middlewares/      # STM32 MCP3204 ADC and MCP2003B LIN drivers
│   ├── projects/         # Nucleo board applications
│   ├── scripts/          # Setup and distribution scripts
│   └── utility/          # Shared utilities
├── arduino-libs/         # Arduino libraries for Inx boards
│   ├── inx_adc_aob/      # ADC Add-on Board library
│   ├── inx_lin_aob/      # LIN Add-on Board library
│   └── scripts/          # Build/flash helper scripts
├── micropython/          # MicroPython module support
└── README.md             # This file
```

## Supported Platforms

### MCU SDK
| Vendor | Target Board | Current examples |
|--------|--------------|------------------|
| STM32  | Nucleo-F446RE | MCP3204 ADC monitor and MCP2003B LIN master |
| STM32  | Nucleo-L552ZE-Q | Cortex-M33 TrustZone Secure/NonSecure CAN FD echo project |

### Arduino
- AVR (Uno, Nano, Mega)
- ARM (SAMD, STM32, etc.)

### MicroPython
- Planned (under development)

## Supported Modules

| Module | MCU SDK | Arduino | MicroPython | Availability |
|--------|---------|---------|-------------|--------------|
| **ADC** | MCP3204 middleware | `InxMcp3204` | Planned | Available |
| **LIN** | MCP2003B middleware | `InxMcp2003b` | Planned | Available |
| **CANFD** | Nucleo-L552ZE-Q sample project | Planned | Planned | Sample project |
| **Motor Driver** | Planned | Planned | Planned | Planned |

## Getting Started

### For MCU SDK (C/C++)
1. See [MCU SDK README](mcu-sdk/README.md) for the current targets and layout.
2. Open a Nucleo sample project under `mcu-sdk/projects/` in STM32CubeIDE, or
   follow the applicable guide in `mcu-sdk/doc/`.

### For Arduino
```bash
# From the repository root, compile an example:
arduino-cli compile --fqbn arduino:avr:uno --libraries arduino-libs \
  arduino-libs/inx_adc_aob/examples/adc_monitor/adc_monitor.ino
```

See [Arduino Libraries README](arduino-libs/Readme.md) for examples.

### For MicroPython
MicroPython board drivers are planned but not yet implemented. See
[MicroPython README](micropython/Readme.md) for the current status.

## Naming Conventions

This project uses **consistent, hierarchical naming** across all implementations:

### C/C++ Code
- **Header Guards**: `INX_<DEVICE>_H` (e.g., `INX_MCP3204_H`)
- **Macros**: `INX_<DEVICE>_<CONSTANT>` (e.g., `INX_MCP3204_VREF_3300MV`)
- **Functions**: `inx_<device>_<action>()` (e.g., `inx_mcp3204_read_raw()`)
- **Types**: `inx_<device>_<name>_t` (e.g., `inx_mcp3204_dev_t`)

### Arduino
- **Classes**: PascalCase (e.g., `InxMcp3204`, `InxMcp2003b`)
- **Methods**: camelCase (e.g., `readRaw()`, `beginMaster()`)

### Documentation
- **Primary README files**: `README.md` (uppercase)
- **Alternative capitalization**: `Readme.md` (used by the Arduino libraries)

## Key Features

✅ **STM32 MCU Support**
- MCP3204 ADC and MCP2003B LIN middleware drivers use STM32 HAL peripherals
- Nucleo-F446RE and Nucleo-L552ZE-Q sample projects are included

✅ **Multiple Integration Paths**
- STM32 C middleware and STM32CubeIDE sample projects
- Arduino libraries with runnable sketches
- MicroPython support planned

✅ **Consistent Device Naming**
- MCU C APIs and Arduino classes use the MCP3204 and MCP2003B device names
- Board projects and libraries follow the `inx_<module>_aob` naming convention

✅ **Examples & Samples**
- Complete working examples for each board
- Both MCU SDK and Arduino implementations
- Per-module documentation

## Contributing

Contributions welcome! When adding new add-on board or features:

1. **Add the implementation** in the appropriate platform-specific middleware.
2. **Add a sample project** under `mcu-sdk/projects/` or an Arduino sketch under
   `arduino-libs/<library>/examples/`.
3. **Document changes** in the relevant README files.

## Architecture Overview

```
MCU applications (STM32CubeIDE projects)
                  |
                  v
STM32 middleware (MCP3204 ADC / MCP2003B LIN)
                  |
                  v
STM32Cube HAL and CMSIS (vendored under mcu-sdk/drivers/)

Arduino applications use the separate libraries in arduino-libs/.
MicroPython support is planned and does not yet contain board drivers.
```

## License

This checkout has no repository-level license file. See the license files
included with vendored dependencies and contact the project maintainer for
licensing information about the SDK itself.

---

**Ikainex Add-on Boards SDK** — Making embedded systems development accessible across platforms.
