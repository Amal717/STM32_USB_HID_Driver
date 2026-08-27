#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "base.h"

typedef enum {
    UART_1 = 0,
    UART_2,
    UART_3,
    UART_4,
    UART_5,
    UART_6,
}uart_t;

typedef enum {
    UART_CLOCK_APB1,
    UART_CLOCK_APB2,
}uart_clock_t;




void uart_init(uart_t uart, uint32_t baudrate);
void uart_putchar_blocking(uart_t uart, char byte);
void uart_putstr_blocking(uart_t uart, const char* str);
void uart_putnum_blocking(uart_t uart, int32_t num, base_t base);
void uart_putfloat_blocking(uart_t uart, float num);
void uart_printf_blocking(uart_t uart, const char *formatstr, ...);
char uart_getchar_blocking(uart_t uart);

#endif // !UART_H_
