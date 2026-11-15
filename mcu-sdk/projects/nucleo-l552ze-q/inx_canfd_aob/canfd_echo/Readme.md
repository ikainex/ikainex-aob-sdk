# CAN FD Echo - NUCLEO-L552ZE-Q

## Example description

This STM32L552ZE-Q application demonstrates CAN FD frame reception and
transmission on FDCAN1. The NonSecure application configures a standard-ID
filter, receives a 12-byte CAN FD frame, prints the received data, copies the
payload, and transmits it using standard ID `0x22`. The configured nominal
bitrate is 500 kbit/s and bitrate switching is disabled.

The project is split into Secure and NonSecure applications for the
Cortex-M33 with TrustZone enabled. The Secure application initializes the
security configuration and transfers control to the NonSecure application.
FDCAN1 and the console are configured in the NonSecure application.

## Keywords

STM32L552ZE-Q, Cortex-M33, TrustZone, Secure, NonSecure, FDCAN, CAN FD

## Directory contents

- `canfd_echo.ioc` - STM32CubeMX configuration for the dual Secure/NonSecure
  project.
- `Secure/Inc/` - Secure application headers, including the TrustZone
  partition configuration.
- `Secure/Src/main.c` - Secure startup, GTZC configuration, and transfer to
  the NonSecure reset handler.
- `Secure/Src/secure_nsc.c` and `Secure_nsclib/` - Secure NonSecure-callable
  support.
- `NonSecure/Inc/` and `NonSecure/Src/` - NonSecure application, including
  FDCAN1 receive/echo handling and LPUART1 console output.
- `STM32CubeIDE/Secure/` and `STM32CubeIDE/NonSecure/` - Separate STM32CubeIDE
  project metadata and linker scripts for both security domains.
- `.mxproject` - STM32CubeIDE project metadata.

## Hardware and software environment

- STM32L552ZE-Q Cortex-M33 on the STMicroelectronics NUCLEO-L552ZE-Q (MB1361)
  board.
- An external CAN FD transceiver and a CAN FD bus are required; PB8/PB9 are
  MCU-level FDCAN signals and must not be connected directly to CANH/CANL.
- FDCAN1 RX is on PB8 and FDCAN1 TX is on PB9.
- LPUART1 TX on PG7 provides console output at 115200 baud through the board's
  onboard ST-LINK USB virtual COM port.
- The project uses STM32CubeL5 V1.6.0 and STM32CubeIDE.

This example has been tested with STMicroelectronics NUCLEO-L552ZE-Q (MB1361)
board and can be easily tailored to any other supported device and development
board.

### CAN connections

| Nucleo-L552ZE-Q pin | CAN FD transceiver pin | Signal |
|---------------------|------------------------|--------|
| PB9 | TXD | MCU transmit |
| PB8 | RXD | MCU receive |
| GND | GND | Common ground |

Connect the transceiver's CANH/CANL pins to the CAN bus. Provide termination at
the bus ends and configure the other nodes for the same nominal bitrate and
CAN FD mode.

## TrustZone configuration

The Secure image starts the NonSecure application at the vector-table address
`0x08040000`. The linker scripts and
`Secure/Inc/partition_stm32l552xx.h` define the memory/security layout. TrustZone
must be enabled for this project.

Configure the STM32L552 option bytes with STM32CubeProgrammer to match the
project partition:

```text
TZEN = 1
DBANK = 1
SECWM1_PSTRT = 0x0   SECWM1_PEND = 0x7F
SECWM2_PSTRT = 0x1   SECWM2_PEND = 0x0
```

These settings make Bank 1 Secure and Bank 2 NonSecure, matching the supplied
linker scripts. Verify the option-byte configuration and the device's current
security state before programming.

## How to build and run

1. Import the `STM32CubeIDE/` folder in STM32CubeIDE. It contains the Secure
   and NonSecure projects.
2. Select the same build configuration (Debug or Release) for both projects.
   Build the NonSecure project; it depends on the Secure project.
3. Configure the Secure project as the startup image and include the
   NonSecure image in the TrustZone debug/run configuration.
4. Connect the Nucleo board over ST-LINK USB, then build, program, and run both
   images with the configured security partitions.
5. Open the board's ST-LINK virtual COM port at 115200 baud. Connect the board
   to a CAN FD network through an external transceiver.

The console prints a startup message, then logs each received frame and the
transmitted payload. The current transmit header uses standard ID `0x22`.

## License

The project uses STM32CubeL5 components; see the vendor license files under
`mcu-sdk/drivers/STM32Cube_FW_L5_V1.6.0/`.
