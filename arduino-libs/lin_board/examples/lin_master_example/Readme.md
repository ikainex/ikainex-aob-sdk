# LIN Add-on Board Sample Application on Arduino UNO for LIN Master

## LIN Add-on Board Connections
This section describes the hardware connections required between the **Arduino UNO** development board and the **LIN** add-on board.

| **UNO Header Pin** | **LIN Add-on Board** | **Description**       |
| ------------------ | -------------------- | --------------------- |
| D1                 | TX                   | MCU TX → LIN Board TX |
| D0                 | RX                   | LIN Board RX → MCU RX |

## `printf` Connection

A **USB-to-TTL Serial Converter** (for example, an **FT232RL FTDI USB-to-TTL UART Adapter Module**) is required to view the serial output.

| **UNO Header Pin** | **USB-to-TTL Converter**   | **Description**                          |
| ---------------    | -------------------------- | -----------------------------------------| 
| D11                | RX                         | `printf` output (MCU TX → USB-to-TTL RX) |
| GND                | GND                        | Common ground                            |

## Output Result
After successful execution, below output is visible.
(Baudrate for serial output: 115200)
```
Lin Commander Start
Sent Data: 1, 2, 3, 4, 5, 6, 7, 8
Lin write message Success
```
## Notes
