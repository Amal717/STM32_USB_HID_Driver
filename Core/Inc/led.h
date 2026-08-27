#ifndef LED_H_

#define LED1_PORT           GPIO_D
#define LED2_PORT           GPIO_D
#define LED3_PORT           GPIO_D
#define LED4_PORT           GPIO_D

#define LED1_PIN            12
#define LED2_PIN            13
#define LED3_PIN            14
#define LED4_PIN            15

typedef enum {
    LED_1 = 1,
    LED_2 = 2,
    LED_3 = 3,
    LED_4 = 4
}led_t;

void led_init();

void led_on(led_t led);
void led_off(led_t led);
void led_toggle(led_t led);


#endif // LED_H_
