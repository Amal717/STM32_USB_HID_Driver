# CMakeLists.tx
cmake_minimum_required(VERSION 3.22)

# --- Project ------------------------------------------------------------
# project() triggers compiler detection using the toolchain file.
# Languages C and ASM declared here - ASM needed for startup_stm32f407xx.s
project(USB_HID_Driver C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Generates compile_commands.json - clangd needs this for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Project     : ${PROJECT_NAME}")
message(STATUS "Build type  : ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler    : ${CMAKE_C_COMPILER}")

# --- Subdirectories ------------------------------------------------------
add_subdirectory(Drivers/HAL)
add_subdirectory(Drivers/USB)
add_subdirectory(Core)

# --- Executable ---------------------------------------------------------
# All sources that produce the final .elf go here.
# startup .s must be here - it contains the reset handler and vector table
# Without it the chip has no entry point on power-on.
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} 
    PRIVATE
        startup_stm32f407xx.s
        Core/Src/main.c
)


# --- Compile defintions ------------------------------------------------
# STM32F407xx tells stm32f4xx.h which registers map to load
# without this the CMSIS headers don't know peripheral base addresses
target_compile_definitions(${PROJECT_NAME}  
    PRIVATE
        STM32F407xx
)

# --- link libraries ----------------------------------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Core
)


# --- Linker options ---------------------------------------------------
# -T    : your memory map - Flash origin, SRAM size, section placement
# -Map  : generates .map file - use this to debug Flash/RAM usage
# These are project-specific so they live here, not in the toolchain file
target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32F407XX_FLASH.ld
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
)

# --- Post-build -------------------------------------------------------
# Runs automatically after every succesfull link
# .elf -> GDB and OpenOCD need this for debugging
# .hex -> Intel HEX format, used by most flash tools (STM32CubeProgrammer)
# .bin -> raw binary, used by custom bootloaders or dd
# size -> prints Flash/RAM breakdown to terminal - check this after every build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex

    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin

    COMMAND ${CMAKE_SIZE} --format=berkeley
        $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating ${PROJECT_NAME}.hex / .bin"
)
