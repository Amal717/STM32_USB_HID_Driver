add_library(USB STATIC)

target_sources(USB
    PRIVATE
        src/usbd_driver.c
        src/usbd_framework.c
)

target_include_directories(USB
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

target_compile_definitions(USB
    PUBLIC
        STM32F407xx
)
