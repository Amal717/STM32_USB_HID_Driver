add_library(HAL STATIC)

target_sources(HAL
    PRIVATE
        src/clock.c
        src/pinmux.c
        src/gpio.c
        src/uart.c
        src/gpio_types.c
)

target_include_directories(HAL
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

target_compile_definitions(HAL
    PUBLIC
        STM32F407xx
)
