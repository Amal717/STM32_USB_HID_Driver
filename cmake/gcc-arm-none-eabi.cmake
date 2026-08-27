# cmake/gcc-arm-none-eabi.cmake
# Toolchain file for ARM Cortex-M4 bare-metal cross-compilation

# --- Target system---------------------------------------------------
set(CMAKE_SYSTEM_NAME   Generic)
set(CMAKE_SYSTEM_PROCESSOR  arm)


# --- Toolchain binaries ---------------------------------------------
set(TOOLCHAIN_PREFIX    arm-none-eabi-)
set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER  ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE          ${TOOLCHAIN_PREFIX}size)

# --- Try-compile guard ----------------------------------------------
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

# --- Output suffix --------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX_C   ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")

# --- CPU flags -----------------------------------------------------
# Must be identical for compiler AND linker - ABI must match acroos all objects
set(TARGET_FLAGS    "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# --- C flags -------------------------------------------------------
set(CMAKE_C_FLAGS
    "${TARGET_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")

# --- ASM flags ------------------------------------------------------
set(CMAKE_ASM_FLAGS
    "${TARGET_FLAGS} -x assembler-with-cpp -MMD -MP")

# --- Build type flags
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")

# --- Linker flags ---------------------------------------------------
# Target_Flags repeated - linker must know CPU to select correct libgcc variant
# Linker script (-T) is NOT here - that is project-specific, set in CMakeLists.txt.
set(CMAKE_EXE_LINKER_FLAGS
    "${TARGET_FLAGS} --specs=nano.specs -Wl,--gc-sections -Wl,--print-memory-usage")
