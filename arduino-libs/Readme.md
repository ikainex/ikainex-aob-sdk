# arduino-libs

Arduino libraries for Ikainex Add-on Boards (e.g. `inx_lin_aob`, `inx_adc_aob`, ...), plus a
build/flash helper script so you don't need the Arduino IDE to compile and upload
examples.

```
arduino-libs/
├── inx_adc_aob/
│   ├── examples
│   │     └── adc_monitor
│   │           └── adc_monitor.ino
│   ├── src
│   │     ├── InxMcp3204.cpp
│   │     └── InxMcp3204.h
│   ├── keywords.txt
│   ├── library.properties
│   └── Readme.md
├── inx_lin_aob/
│   ├── examples
│   │     ├── lin_master_transmit
│   │     │     └── lin_master_transmit.ino
│   │     └── lin_slave_example
│   │           └── lin_slave_example.ino
│   ├── src
│   │     ├── InxMcp2003b.cpp
│   │     └── InxMcp2003b.h
│   ├── keywords.txt
│   ├── library.properties
│   └── Readme.md
├── scripts
│   └── run_ikainex_aob_examples.bat
└── Readme.md
```

Each `inx_<module>_aob` folder (`inx_adc_aob`, `inx_lin_aob`, ...) is a **self-contained
Arduino library** in the standard Arduino library layout, so it also works if you
open it directly in the Arduino IDE. See each board's own `Readme.md` for
board-specific wiring, API, and usage notes.

---

## 1. Prerequisites

1. **arduino-cli** installed and available on your `PATH`.  
   Install from: https://arduino.github.io/arduino-cli/latest/installation/   
   Verify with:
   ```
   arduino-cli version
   ```
2. **Board core installed** for whichever Arduino board you're targeting, e.g.:
   ```
   arduino-cli core install arduino:avr        # Uno / Nano / Mega
   ```
3. Know which **COM port** your board is on:
   ```
   arduino-cli board list
   ```

No manual `--libraries` setup is needed — the build script automatically points
`arduino-cli` at the `arduino-libs` folder so any Ikainex add-on board library can `#include` any
other one if required.

---

## 2. Quick Start

From the repository root, run the build/flash script and select the library and
example:

```bat
scripts/run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --com COM3
```

This will **build and flash by default**. That's it — no path juggling required.

---

## 3. Repository Layout Conventions

Every add-on board library must follow this layout so the build script and Arduino
IDE can both find things automatically:

| Path | Purpose |
|------|---------|
| `inx_<module>_aob/src/` | Library source (`.cpp` / `.h`) |
| `inx_<module>_aob/examples/<example>/<example>.ino` | One folder per sketch; folder and sketch names match |
| `inx_<module>_aob/library.properties` | Arduino library metadata |
| `inx_<module>_aob/keywords.txt` | Arduino IDE keyword highlighting |
| `inx_<module>_aob/Readme.md` | Board wiring, API, and examples |


### Adding a new Annex board library

1. Create `inx_<module>_aob/` with `src/`, `examples/`, `library.properties`,
   `keywords.txt`, and `Readme.md` following the layout above.
2. Include `Arduino.h` in source files that use Arduino core APIs; library
   `.cpp` files do not get it implicitly like `.ino` sketches do.
3. Add at least one working example under `examples/<example>/<example>.ino`.
4. Document the board in its own `Readme.md` (pinout, constructor/API, example
   usage).
5. Test it with the build script:
   ```bat
   scripts\run_ikainex_aob_examples.bat --aob inx_<module>_aob --example <example> --com <COMx>
   ```

---

## 4. Build / Flash Script Reference

Script location: `scripts\run_ikainex_aob_examples.bat`

By default it **builds and flashes**. Add `--build-only` to just compile.

```bat
run_ikainex_aob_examples.bat [clean] --aob <name> --example <name> ^
                 [--arduino-board <board>] [--com <port>] [--build-only]
```

| Argument            | Required | Default | Description                                                                 |
|---------------------|----------|---------|-------------------------------------------------------------------------------|
| `--aob`     | Yes      | `inx_lin_aob` | Annex library folder name, e.g. `inx_lin_aob`, `inx_adc_aob`             |
| `--example`          | Yes      | —       | Example name only (no path, no `.ino`) — e.g. `lin_master_transmit` |
| `--arduino-board`   | No       | `uno`   | Target board shorthand or full FQBN — see table below                     |
| `--com`             | No       | `COM15` | Serial port to flash, e.g. `COM3`                                          |
| `--build-only`      | No       | off     | Compile only, skip flashing                                                |
| `clean`             | No       | —       | Remove that board's build output folder and exit                          |
| `--help` / `-h`     | No       | —       | Show usage                                                                  |

### `--arduino-board` shorthand → FQBN mapping

| Shorthand       | FQBN                        |
|-----------------|------------------------------|
| `uno`           | `arduino:avr:uno`            |
| `nano`          | `arduino:avr:nano`           |
| `mega`          | `arduino:avr:mega`           |
| `mkr1000`       | `arduino:samd:mkr1000`       |
| `nano_33_iot`   | `arduino:samd:nano_33_iot`   |
| `esp32`         | `esp32:esp32:esp32`          |
| `rpipico`       | `rp2040:rp2040:rpipico`      |

You can also pass a full FQBN directly (anything containing `:`), e.g.
`--arduino-board arduino:avr:leonardo`.

### Examples

```bat
:: Build + flash inx_lin_aob's lin_master_transmit to an Uno on COM3
run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --com COM3

:: Build + flash inx_adc_aob's adc_monitor to a Mega on COM5
run_ikainex_aob_examples.bat --aob inx_adc_aob --example adc_monitor --arduino-board mega --com COM5

:: Just compile, don't flash (e.g. CI / sanity check)
run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --build-only

:: Clean build output for a given board/example combo
run_ikainex_aob_examples.bat clean --aob inx_lin_aob --example lin_master_transmit
```

Build output for each board/FQBN combination goes to:
```
scripts\build\<fqbn-with-colons-replaced-by-underscores>\
```

---

## 5. Per-Board Documentation

For wiring diagrams and API references, see the `Readme.md` in each library.
Example-specific wiring and output notes are documented beside the LIN sketches:

- [`inx_adc_aob/Readme.md`](./inx_adc_aob/Readme.md)
- [`inx_lin_aob/Readme.md`](./inx_lin_aob/Readme.md)
- [`lin_master_transmit/Readme.md`](./inx_lin_aob/examples/lin_master_transmit/Readme.md)
- [`lin_slave_example/Readme.md`](./inx_lin_aob/examples/lin_slave_example/Readme.md)