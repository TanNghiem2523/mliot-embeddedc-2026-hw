cmake_minimum_required(VERSION 3.22)

#
# Baremetal STM32F103 GPIO_Button Project
# Structured following the standard STM32Cube 3-layer architecture:
#   1) CMSIS Core       -> Drivers/CMSIS/Include
#   2) CMSIS Device     -> Drivers/CMSIS/Device/ST/STM32F1xx/Include + Source
#   3) HAL Driver       -> Drivers/STM32F1xx_HAL_Driver/Inc + Src
#   4) Application code -> project root (main.c, stm32f1xx_hal_conf.h, ...)
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME GPIO_Button)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# ---------------------------------------------------------------------------
# 1) CMSIS Core (ARM generic: core_cm3.h, cmsis_gcc.h, cmsis_version.h ...)
# ---------------------------------------------------------------------------
set(CMSIS_CORE_Dir
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

# ---------------------------------------------------------------------------
# 2) CMSIS Device (ST STM32F1xx: stm32f1xx.h, stm32f103xb.h, system_stm32f1xx.h)
# ---------------------------------------------------------------------------
set(CMSIS_DEVICE_Include_Dir
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
)

set(CMSIS_DEVICE_Src
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
)

# Startup assembly file (adjust name if your chip variant differs)
set(STARTUP_Src
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s
)

# ---------------------------------------------------------------------------
# 3) HAL Driver (ST STM32F1xx HAL: stm32f1xx_hal.h/.c, stm32f1xx_hal_gpio.h/.c ...)
# ---------------------------------------------------------------------------
set(HAL_DRIVER_Include_Dir
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
)

set(HAL_DRIVER_Src
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
)

# ---------------------------------------------------------------------------
# 4) Application layer (project root: main.c, main.h, stm32f1xx_hal_conf.h ...)
# ---------------------------------------------------------------------------
set(APP_Include_Dir
    ${CMAKE_SOURCE_DIR}
)

set(APP_Src
    ${CMAKE_SOURCE_DIR}/main.c
)

# ---------------------------------------------------------------------------
# Preprocessor defines
# ---------------------------------------------------------------------------
set(BAREMETAL_Defines_Syms
    STM32F103xB
    USE_FULL_ASSERT
    HSI_VALUE=8000000
    $<$<CONFIG:Debug>:DEBUG>
)

# ---------------------------------------------------------------------------
# Collect all include directories (order matters: app first, then device/HAL/core)
# ---------------------------------------------------------------------------
set(BAREMETAL_Include_Dirs
    ${APP_Include_Dir}
    ${HAL_DRIVER_Include_Dir}
    ${CMSIS_DEVICE_Include_Dir}
    ${CMSIS_CORE_Dir}
)

# ---------------------------------------------------------------------------
# Collect all sources
# ---------------------------------------------------------------------------
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${APP_Src}
    ${HAL_DRIVER_Src}
    ${CMSIS_DEVICE_Src}
    ${STARTUP_Src}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${BAREMETAL_Include_Dirs})

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${BAREMETAL_Defines_Syms}
)

# Post-build commands to generate hex and bin files
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Building HEX and BIN files"
)

# Print executable size
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "Program size:"
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    ADDITIONAL_CLEAN_FILES "${CMAKE_PROJECT_NAME}.map;${CMAKE_PROJECT_NAME}.hex;${CMAKE_PROJECT_NAME}.bin"
)

# Validate that code is compatible with C standard
if((CMAKE_C_STANDARD LESS 11))
    message(FATAL_ERROR "Generated code requires C11 or higher")
endif()
