#ifndef PINMUX_H_
#define PINMUX_H_

#include "gpio_types.h"

typedef enum {
AF0 = 0,    /* → System */
AF1,        /* → TIM1/TIM2 */
AF2,        /* → TIM3/TIM4/TIM5 */
AF3,        /* → TIM8/TIM9/TIM10/TIM11 */
AF4,        /* → I2C1/I2C2/I2C3 */
AF5,        /* → SPI1/SPI2/I2S */
AF6,        /* → SPI3/I2S */
AF7,        /* → USART1/USART2/USART3 */
AF8,        /* → UART4/UART5/USART6 */
AF9,        /* → CAN1/CAN2/TIM12/13/14 */
AF10,       /* → USB OTG */
AF11,       /* → Ethernet */
AF12,       /* → FSMC/SDIO/USB */
AF13,       /* → DCMI */
AF14,       /* → reserved/none for these mappings */
AF15,       /* → EVENTOUT */
}pin_af_t;


typedef struct {
    gpio_port_t port;
    uint8_t     pin;
    uint8_t     af;
}pinconfig_t;

void pinmux_init(void);

#endif /* PINMUX_H_ */
