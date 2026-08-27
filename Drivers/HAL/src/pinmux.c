#include "pinmux.h"

void pinmux_sel(gpio_port_t port, uint8_t pin, uint8_t af);
void gpio_clock_enable(gpio_port_t port);

const pinconfig_t uart2_pins[] = {
    { GPIO_A, 2, AF7 },  // PA2 -> USART2_TX
    { GPIO_A, 3, AF7 }   // PA3 -> USART2_RX
};

const pinconfig_t usb_pins[] = {
    { GPIO_A, 11, AF10 },
    { GPIO_A, 12, AF10 },
};

void pinmux_sel(gpio_port_t port, uint8_t pin, uint8_t af)
{
    GPIO_TypeDef *GPIOx = gpio_regs[port];
    uint8_t pos;

    /* Step 1: Set GPIOx mode = Alternate Function (10) */
    GPIOx->MODER &= ~(3U << (pin * 2));
    GPIOx->MODER |= (2U << (pin * 2));

    /* Step 2: Select AFRL or AFRH */
    if (pin < 8) {
        /* Pin 0 - 7 -> AFRL */
        pos = pin * 4;
        GPIOx->AFR[0] &= ~(0xFU << pos);
        GPIOx->AFR[0] |= ((uint32_t)af << pos);
    } else {
        /* Pin 8 - 15 -> AFRH */
        pos = (pin - 8) * 4;
        GPIOx->AFR[1] &= ~(0xFU << pos);
        GPIOx->AFR[1] |= ((uint32_t)af << pos);
    }
}

void gpio_clock_enable(gpio_port_t port)
{
    RCC->AHB1ENR |= gpio_clock_bits[port];
}

void pinmux_init(void)
{
    gpio_clock_enable(GPIO_A); /* UART USB */

    pinmux_sel(uart2_pins[0].port, uart2_pins[0].pin, uart2_pins[0].af); /* PA2 -> USART2_TX */
    pinmux_sel(uart2_pins[1].port, uart2_pins[1].pin, uart2_pins[1].af); /* PA3 -> USART2_RX */

    pinmux_sel(usb_pins[0].port, usb_pins[0].pin, usb_pins[0].af);   /* USB_D- */
    pinmux_sel(usb_pins[1].port, usb_pins[1].pin, usb_pins[1].af);   /* USB_D+ */


}
