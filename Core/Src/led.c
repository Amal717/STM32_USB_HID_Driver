#include "led.h"
#include "gpio.h"


void led_init()
{
    gpio_init(LED1_PORT, LED1_PIN, GPIO_OUTPUT);
    gpio_init(LED2_PORT, LED2_PIN, GPIO_OUTPUT);
    gpio_init(LED3_PORT, LED3_PIN, GPIO_OUTPUT);
    gpio_init(LED4_PORT, LED4_PIN, GPIO_OUTPUT);
}

void led_on(led_t led)
{
    switch (led) {
        case LED_1:
            gpio_set_pin(LED1_PORT, LED1_PIN);
            break;
        case LED_2:
            gpio_set_pin(LED2_PORT, LED2_PIN);
            break;
        case LED_3:
            gpio_set_pin(LED3_PORT, LED3_PIN);
            break;
        case LED_4:
            gpio_set_pin(LED4_PORT, LED4_PIN);
            break;

        default:
            /* do nothing */
            break;
    }
}

void led_off(led_t led)
{
    switch (led) {
        case LED_1:
            gpio_clr_pin(LED1_PORT, LED1_PIN);
            break;
        case LED_2:
            gpio_clr_pin(LED2_PORT, LED2_PIN);
            break;
        case LED_3:
            gpio_clr_pin(LED3_PORT, LED3_PIN);
            break;
        case LED_4:
            gpio_clr_pin(LED4_PORT, LED4_PIN);
            break;

        default:
            /* do nothing */
            break;
    }
}

void led_toggle(led_t led)
{
    switch (led) {
        case LED_1:
            gpio_toggle(LED1_PORT, LED1_PIN);
            break;

        case LED_2:
            gpio_toggle(LED2_PORT, LED2_PIN);
            break;

        case LED_3:
            gpio_toggle(LED3_PORT, LED3_PIN);
            break;

        case LED_4:
            gpio_toggle(LED4_PORT, LED4_PIN);
            break;

        default:
            /* do nothing */
            break;
    
    }

}
