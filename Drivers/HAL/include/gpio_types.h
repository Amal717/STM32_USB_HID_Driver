#ifndef GPIO_TYPES_H_
#define GPIO_TYPES_H_

#include "stm32f407xx.h"

typedef enum {
    GPIO_A = 0,
    GPIO_B,
    GPIO_C,
    GPIO_D,
    GPIO_E,
    GPIO_F,
    GPIO_G,
    GPIO_H,
    GPIO_I
}gpio_port_t;

extern GPIO_TypeDef * const gpio_regs[];

extern uint32_t const gpio_clock_bits[];

#endif // !GPIO_TYPES_H_
