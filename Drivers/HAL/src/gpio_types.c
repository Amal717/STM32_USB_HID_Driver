#include "gpio_types.h"

GPIO_TypeDef* const  gpio_regs[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH,
    GPIOI,
};

uint32_t const gpio_clock_bits[] = {
    RCC_AHB1ENR_GPIOAEN,
    RCC_AHB1ENR_GPIOBEN,
    RCC_AHB1ENR_GPIOCEN,
    RCC_AHB1ENR_GPIODEN,
    RCC_AHB1ENR_GPIOEEN,
    RCC_AHB1ENR_GPIOFEN,
    RCC_AHB1ENR_GPIOGEN,
    RCC_AHB1ENR_GPIOHEN,
    RCC_AHB1ENR_GPIOIEN
};

