@echo off
setlocal enabledelayedexpansion

echo STEP 1: CLEANING BUILD DIRECTORY
if exist "build" (
    rmdir /s /q "build"
    if errorlevel 1 (
        echo       ERROR: Failed to delete "build" directory.
        goto :error
    )
    echo       Old "build" directory removed successfully.
) else (
    echo       No existing "build" directory found. Skipping.
)
echo.

echo STEP 2: CONFIGURING PROJECT WITH CMAKE
cmake -S . -B build -G "Ninja"
if errorlevel 1 (
    echo       ERROR: CMake configuration failed.
    goto :error
)
echo       CMake configuration completed successfully.
echo.

echo STEP 3: COMPILING FIRMWARE WITH NINJA
ninja -C build
if errorlevel 1 (
    echo       ERROR: Ninja build failed.
    goto :error
)
echo       Build completed successfully. Output: build\app_firmware.bin
echo.

echo STEP 4: FLASHING FIRMWARE TO TARGET MCU
STM32_Programmer_CLI -c port=SWD -w "build\GPIO_Button.bin" 0x08000000 -rst
if errorlevel 1 (
    echo       ERROR: Flashing failed.
    goto :error
)
echo       Flash completed successfully. Chip has been reset and is running the new firmware.
echo.

goto :end

:error
echo.
echo ============================================
echo   SCRIPT FAILED - See error message above
echo ============================================
pause
exit /b 1

:end
endlocal
exit /b 0