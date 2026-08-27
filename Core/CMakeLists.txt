add_library(Core STATIC)

# Add sources to executable
target_sources(Core
    PRIVATE
        Src/system_stm32f4xx.c
        Src/led.c
)


# --- Include paths ------------------------------------------------------
# CMSIS/Include     -> core_cm4.h, cmsis_gcc.h (ARM core peripherals)
# STM32F4xx/Include -> stm32f407xx.h (peripheral register maps)
# Inc               -> your own headers
target_include_directories(Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)


# --- Link library -------------------------------------------------------
target_link_libraries(Core
    PUBLIC
        HAL
        USB
)

