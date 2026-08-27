#ifndef GPIO_H_
#include "gpio_types.h"

typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT,
    ANALOG_MODE
}gpio_mode_t;



void gpio_init(gpio_port_t port, uint8_t pin, gpio_mode_t mode);
void gpio_set_pin(gpio_port_t port, uint8_t pin);
void gpio_clr_pin(gpio_port_t port, uint8_t pin);
void gpio_toggle(gpio_port_t port, uint8_t pin);
uint8_t gpio_read(gpio_port_t port, uint8_t pin);

#endif // !GPIO_H_
