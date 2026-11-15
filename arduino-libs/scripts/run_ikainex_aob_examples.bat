@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: ============================================================
::  Arduino build/upload script for Windows
::
::  Default behavior: BUILD + FLASH (upload)
::  Use --build-only to skip flashing.
::
::  Usage:
::    run_ikainex_aob_examples.bat [clean] --aob <name> --example <name> ^
::                       [--arduino-board <board>] [--com <port>] [--build-only]
::
::  Example path is auto-resolved as:
::    <SDK_ROOT>\<aob-board>\examples\<example>\<example>.ino
::
::  Examples:
::    run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --com COM3
::    run_ikainex_aob_examples.bat --aob inx_adc_aob --example adc_monitor --arduino-board mega --com COM5
::    run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --build-only
::    run_ikainex_aob_examples.bat clean --aob inx_lin_aob --example lin_master_transmit
:: ============================================================

:: ── Anchor everything to the script location ─────────────────
set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "SDK_ROOT=%%~fI"

:: ------------------------------------------------------------
::  SECTION 1: arduino-cli path
:: ------------------------------------------------------------

:: Option A: If arduino-cli is on your system PATH, just use the name
set "ARDUINO_CLI=arduino-cli"

:: Option B: Full path if not on PATH (uncomment and edit)
:: set "ARDUINO_CLI=C:\Users\YourName\AppData\Local\Programs\arduino-cli\arduino-cli.exe"

:: ------------------------------------------------------------
::  SECTION 2: Defaults
:: ------------------------------------------------------------

set "AOB=inx_lin_aob"
set "ARDUINO_BOARD=uno"
set "FQBN=arduino:avr:uno"
set "PORT=COM15"
set "EXAMPLE_NAME="

:: ------------------------------------------------------------
::  SECTION 3: Build / flash mode  (default = build + flash)
:: ------------------------------------------------------------

set "DO_CLEAN=0"
set "DO_BUILD=1"
set "DO_UPLOAD=1"

:: ------------------------------------------------------------
::  SECTION 4: Resolved paths (filled in later)
:: ------------------------------------------------------------

set "EXAMPLE="
set "BUILD_DIR="

:: ------------------------------------------------------------
::  SECTION 5: Parse arguments
:: ------------------------------------------------------------

:parse_args
if "%~1"=="" goto args_done
set "ARG=%~1"

if /I "%ARG%"=="clean" (
    set "DO_CLEAN=1"
    set "DO_BUILD=0"
    set "DO_UPLOAD=0"
    shift
    goto parse_args
)
if /I "%ARG%"=="--build-only" (
    set "DO_UPLOAD=0"
    shift
    goto parse_args
)
if /I "%ARG%"=="--help" goto usage
if /I "%ARG%"=="-h" goto usage

if /I "%ARG%"=="--aob" (
    if "%~2"=="" goto usage
    set "AOB=%~2"
    shift
    shift
    goto parse_args
)
if /I "%ARG%"=="--arduino-board" (
    if "%~2"=="" goto usage
    set "ARDUINO_BOARD=%~2"
    shift
    shift
    goto parse_args
)
if /I "%ARG%"=="--com" (
    if "%~2"=="" goto usage
    set "PORT=%~2"
    shift
    shift
    goto parse_args
)
if /I "%ARG%"=="--example" (
    if "%~2"=="" goto usage
    set "EXAMPLE_NAME=%~2"
    shift
    shift
    goto parse_args
)

echo [ERROR] Unrecognized argument: %ARG%
goto usage

:args_done

:: ------------------------------------------------------------
::  SECTION 6: Map --arduino-board shorthand to FQBN
:: ------------------------------------------------------------

set "FQBN=%ARDUINO_BOARD%"
if /I "%ARDUINO_BOARD%"=="uno"          set "FQBN=arduino:avr:uno"
if /I "%ARDUINO_BOARD%"=="nano"         set "FQBN=arduino:avr:nano"
if /I "%ARDUINO_BOARD%"=="mega"         set "FQBN=arduino:avr:mega"
if /I "%ARDUINO_BOARD%"=="mkr1000"      set "FQBN=arduino:samd:mkr1000"
if /I "%ARDUINO_BOARD%"=="nano_33_iot"  set "FQBN=arduino:samd:nano_33_iot"
if /I "%ARDUINO_BOARD%"=="esp32"        set "FQBN=esp32:esp32:esp32"
if /I "%ARDUINO_BOARD%"=="rpipico"      set "FQBN=rp2040:rp2040:rpipico"
:: If ARDUINO_BOARD already looks like a full FQBN (contains ':'), it passes through unchanged.

:: ------------------------------------------------------------
::  SECTION 7: Require --example, then resolve example path
:: ------------------------------------------------------------

if not defined EXAMPLE_NAME (
    echo [ERROR] --example is required.
    echo         Example: --example lin_master_transmit
    goto usage
)

set "EXAMPLE_PATH=%AOB%\examples\%EXAMPLE_NAME%\%EXAMPLE_NAME%.ino"
for %%I in ("%SDK_ROOT%\%EXAMPLE_PATH%") do set "EXAMPLE=%%~fI"

set "BUILD_DIR=%SCRIPT_DIR%build\%FQBN::=_%"
set "BUILD_PROPERTY_C=compiler.c.extra_flags=%EXTRA_C_FLAGS% %EXTRA_DEFINES%"
set "BUILD_PROPERTY_CPP=compiler.cpp.extra_flags=%EXTRA_CPP_FLAGS% %EXTRA_DEFINES%"
set "LIB_FLAGS=--libraries "%SDK_ROOT%""

:: ── Header ──────────────────────────────────────────────────
echo.
echo ============================================================
echo   Arduino Build / Flash Script
echo ============================================================
echo   Add-on Board    : %AOB%
echo   Example         : %EXAMPLE_NAME%
echo   Arduino Board   : %ARDUINO_BOARD%  (%FQBN%)
echo   Example Path    : %EXAMPLE%
echo   Output          : %BUILD_DIR%
echo   Port            : %PORT%
echo   Mode            : Clean=%DO_CLEAN%  Build=%DO_BUILD%  Flash=%DO_UPLOAD%
echo ============================================================
echo.

:: ── Check arduino-cli exists ─────────────────────────────────
%ARDUINO_CLI% version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] arduino-cli not found.
    echo         Install from: https://arduino.github.io/arduino-cli/latest/installation/
    echo         Or set ARDUINO_CLI to the full path in this script.
    goto fail
)

for /f "tokens=*" %%v in ('%ARDUINO_CLI% version 2^>^&1') do echo   CLI     : %%v
echo.

:: ── Clean ────────────────────────────────────────────────────
if "%DO_CLEAN%"=="1" (
    echo [STEP] Cleaning build directory...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
        echo        Removed: %BUILD_DIR%
    ) else (
        echo        Nothing to clean.
    )
    echo.
    if "%DO_BUILD%"=="0" goto done
)

:: ── Check example exists ──────────────────────────────────────
if not exist "%EXAMPLE%" (
    echo [ERROR] Example not found: %EXAMPLE%
    echo         Expected layout: ^<aob-board^>\examples\^<example^>\^<example^>.ino
    goto fail
)

:: ── Create build dir ─────────────────────────────────────────
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: ── Compile ──────────────────────────────────────────────────
echo [STEP] Compiling...
echo        Example : %EXAMPLE%
echo        Board  : %FQBN%
echo.

%ARDUINO_CLI% compile ^
    --fqbn "%FQBN%" ^
    %LIB_FLAGS% ^
    --build-property "%BUILD_PROPERTY_C%" ^
    --build-property "%BUILD_PROPERTY_CPP%" ^
    --build-path "%BUILD_DIR%" ^
    --verbose ^
    "%EXAMPLE%"

if errorlevel 1 (
    echo.
    echo [FAILED] Compilation failed. Check errors above.
    goto fail
)

echo.
echo [OK] Compilation successful.
echo      Output files in: %BUILD_DIR%
echo.

:: ── Show binary size ─────────────────────────────────────────
echo [INFO] Build artifacts:
for %%f in ("%BUILD_DIR%\*.hex" "%BUILD_DIR%\*.elf" "%BUILD_DIR%\*.bin") do (
    if exist "%%f" (
        for %%s in ("%%f") do echo        %%~nxf  ^(%%~zs bytes^)
    )
)
echo.

:: ── Upload (flash) ──────────────────────────────────────────
if "%DO_UPLOAD%"=="1" (
    echo [STEP] Flashing to %PORT%...
    echo.

    %ARDUINO_CLI% upload ^
        --fqbn "%FQBN%" ^
        --port "%PORT%" ^
        --input-dir "%BUILD_DIR%" ^
        --verbose ^
        "%EXAMPLE%"

    if errorlevel 1 (
        echo.
        echo [FAILED] Flash failed.
        echo          Check PORT=%PORT% is correct: arduino-cli board list
        goto fail
    )

    echo.
    echo [OK] Flash successful.
    echo.
) else (
    echo [INFO] --build-only specified, skipping flash step.
    echo.
)

:done
echo ============================================================
echo   Done.
echo ============================================================
echo.
exit /b 0

:fail
echo ============================================================
echo   Build FAILED.
echo ============================================================
echo.
exit /b 1

:usage
echo.
echo Usage:
echo   run_ikainex_aob_examples.bat [clean] --aob ^<inx_lin_aob^|inx_adc_aob^|...^> --example ^<example_name^> ^^
echo                      [--arduino-board ^<uno^|mega^|nano^|esp32^|...^|full-fqbn^>] ^^
echo                      [--com ^<COMx^>] [--build-only]
echo.
echo Notes:
echo   - Default action is BUILD + FLASH. Add --build-only to only build.
echo   - --example expects just the example name, not a path. The .ino path is
echo     resolved automatically as:
echo         ^<aob^>\examples\^<example^>\^<example^>.ino
echo   - Defaults: --arduino-board uno, --com COM15
echo.
echo Examples:
echo   run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --com COM3
echo   run_ikainex_aob_examples.bat --aob inx_adc_aob --example adc_monitor --arduino-board mega --com COM5
echo   run_ikainex_aob_examples.bat --aob inx_lin_aob --example lin_master_transmit --build-only
echo   run_ikainex_aob_examples.bat clean --aob inx_lin_aob --example lin_master_transmit
echo.
exit /b 0