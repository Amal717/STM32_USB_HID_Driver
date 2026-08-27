#include <stdint.h>
#include "clock.h"
#include "pinmux.h"
#include "led.h"
#include "uart.h"
#include "usbd_framework.h"

void delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
        for (uint32_t j = 0; j < 12000; j++);
}


int main(void)
{
    clock_init();
    pinmux_init();
    led_init();
    uart_init(UART_2, 9600);
    usbd_init();

    while (1)
    {
        led_on(LED_1);
        delay(500);
        led_off(LED_1);
        delay(500);

    }
    return 0;
}
