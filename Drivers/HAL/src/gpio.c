#include "gpio.h"


void gpio_init(gpio_port_t port, uint8_t pin, gpio_mode_t mode)
{

    RCC->AHB1ENR |= gpio_clock_bits[port];
    
    switch (mode) {

        case GPIO_INPUT:
            gpio_regs[port]->MODER &= ~(3U << pin * 2);
            break;

        case GPIO_OUTPUT:
            gpio_regs[port]->MODER |= (1U << pin *2);
            break;

        case ANALOG_MODE:
            gpio_regs[port]->MODER |= (3U << pin * 2);

        default:
            /* do nothing */
            break;
    }
}

uint8_t gpio_read(gpio_port_t port, uint8_t pin)
{
    return (gpio_regs[port]->IDR >> pin) & 0x01;
}

void gpio_set_pin(gpio_port_t port, uint8_t pin)
{
    gpio_regs[port]->BSRR |=  1 << pin;
}

void gpio_clr_pin(gpio_port_t port, uint8_t pin)
{
    gpio_regs[port]->BSRR |= 1 << (pin + 16);
}

void gpio_toggle(gpio_port_t port, uint8_t pin)
{
    uint8_t led_state = (gpio_regs[port]->ODR >> pin) & 0x01;

    if (led_state == 1)
        gpio_clr_pin(port, pin);
    else
        gpio_set_pin(port, pin);
}
