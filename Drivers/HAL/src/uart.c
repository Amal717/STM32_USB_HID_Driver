#include <stdarg.h>
#include "uart.h"
#include "clock.h"
#include "stm32f407xx.h"

static USART_TypeDef *uart_reg[] =
{
    USART1,
    USART2,
    USART3,
    UART4,
    UART5,
    USART6
};

static uart_clock_t uart_clock[] = {
    UART_CLOCK_APB2,    /* UART_1 */
    UART_CLOCK_APB1,    /* UART_2 */
    UART_CLOCK_APB1,    /* UART_3 */
    UART_CLOCK_APB1,    /* UART_4 */
    UART_CLOCK_APB1,    /* UART_5 */
    UART_CLOCK_APB2,    /* UART_6 */
};


#define UART_OVERSAMPLING           16U
#define UART_BRR_FRAC               16U

typedef void (*uart_putchar_fn_t)(uart_t uart, char byte);
typedef void (*uart_putstr_fn_t)(uart_t uart, const char* str);

void uart_clocK_enable(uart_t uart)
{
    switch (uart) {

        case UART_1:
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            break;

        case UART_2:
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
            break;

        case UART_3:
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
            break;

        case UART_4:
            RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
            break;

        case UART_5:
            RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
            break;

        case UART_6:
            RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
            break;

        default:
            /* Do Nothing */
            break;
    }
}

static uint32_t uart_get_clock(uart_t uart)
{
    switch (uart_clock[uart]) {
        
        case UART_CLOCK_APB1:
            return PCLK1_HZ;

        case UART_CLOCK_APB2:
            return PCLK2_HZ;

        default:
            /* Do Nothing */
            break;
    }
}

static void uart_set_baudrate(uart_t uart, uint32_t baudrate)
{
    uint32_t pclk;
    uint32_t divisor;
    uint32_t mantissa;
    uint32_t remainder;
    uint32_t fraction;

    pclk = uart_get_clock(uart);
    divisor = UART_OVERSAMPLING * baudrate;
    mantissa = pclk / divisor;
    remainder = pclk % divisor;

    fraction = (remainder * UART_BRR_FRAC + divisor / 2U) / divisor;

    if (fraction == 16U) {
        mantissa++;
        fraction = 0U;
    }

    uart_reg[uart]->BRR = 
        (mantissa << USART_BRR_DIV_Mantissa_Pos) |
        (fraction << USART_BRR_DIV_Fraction_Pos) ;
}

static void uart_configure(uart_t uart)
{
    /* 8-bit data, no parity */
    uart_reg[uart]->CR1 &= ~(USART_CR1_M | USART_CR1_PCE);

    /* 1 stop bit */
    uart_reg[uart]->CR2 &= ~USART_CR2_STOP;

    /* Enable transmitter and reciever */
    uart_reg[uart]->CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* Enable USART */
    uart_reg[uart]->CR1 |= USART_CR1_UE;
}

void uart_init(uart_t uart, uint32_t baudrate)
{
    uart_clocK_enable(uart);
    uart_set_baudrate(uart, baudrate);
    uart_configure(uart);
}

void uart_putchar_blocking(uart_t uart, char byte)
{
    while ((uart_reg[uart]->SR & USART_SR_TXE) == 0U);
    uart_reg[uart]->DR = byte;
}

static void uart_putstr(uart_t uart, const char* str, uart_putchar_fn_t uart_putchar)
{
    while(*str != '\0')
        uart_putchar(uart, *str++);
}

static void uart_putnum(uart_t uart, int32_t num, base_t base, uart_putchar_fn_t uart_putchar, uart_putstr_fn_t uart_putstr)
{
    char buf[20];
    uint8_t i = 0;

    if(num < 0)
    {
       uart_putchar(uart, '-');
       num = -num;
    }

    do
    {
        int digit = num % base;
        buf[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
        num /= base;
    }while( num > 0);

    buf[i] = '\0';

    uint8_t start = 0;
    uint8_t end = i - 1;
    while(start < end)
    {
        char temp = buf[start];
        buf[start++] = buf[end];
        buf[end--] = temp;
    }
    uart_putstr(uart, buf);
}


static void uart_putfloat(uart_t uart, float num, uart_putchar_fn_t uart_putchar, uart_putstr_fn_t uart_putstr)
{
    char buf[20];
    uint8_t i = 0;

    if(num < 0.0f)
    {
        uart_putchar(uart, '-');
        num = -num;
    }

    int32_t int_part = (int32_t)num;
    float frac_part = num - (float)int_part;

    /* convert integer digits */
    do
    {
        buf[i++] = (int_part % 10) + '0';
        int_part /= 10;
    }while(int_part > 0);

    /* reverse integer digits */
    uint8_t start = 0;
    uint8_t end = i - 1;
    while(start < end)
    {
        char temp = buf[start];
        buf[start++] = buf[end];
        buf[end--] = temp;
    }

    buf[i++] = '.';
    uint8_t precision = 3;
    while(precision--)
    {
        frac_part *= 10;
        uint8_t digit = (uint8_t)frac_part;
        buf[i++] = digit + '0';
        frac_part -= (float)digit;
    }
    buf[i] = '\0';
    uart_putstr(uart, buf);
}

void uart_putstr_blocking(uart_t uart, const char* str)
{
    uart_putstr(uart, str, uart_putchar_blocking);
}

void uart_putnum_blocking(uart_t uart, int32_t num, base_t base)
{
    uart_putnum(uart, num, base, uart_putchar_blocking,  uart_putstr_blocking);
}

void uart_putfloat_blocking(uart_t uart, float num)
{
    uart_putfloat(uart, num, uart_putchar_blocking, uart_putstr_blocking);
}

void uart_printf_blocking(uart_t uart, const char *formatstr, ...)
{
    int num = 0;
    double value = 0.0;
    char *str;

    /* Declaring an object for va_list called as arglist */
    va_list arglist;
    va_start(arglist, formatstr);

    while(*formatstr != '\0')
    {
        if(*formatstr != '%')
            uart_putchar_blocking(uart, *formatstr);
        else
        {
            formatstr++;

            switch (*formatstr)
            {
                case 'c':
                num = va_arg(arglist, int);
                uart_putchar_blocking(uart, num);
                break;

                case 'd':
                num = va_arg(arglist, int);
                uart_putnum_blocking(uart, num, DEC);
                break;

                case 'f':
                value = va_arg(arglist, double);
                uart_putfloat_blocking( uart, (float)value);
                break;

                case 'o':
                num = va_arg(arglist, int);
                uart_putnum_blocking(uart, num, OCT);
                break;

                case 's':
                str = va_arg(arglist, char*);
                uart_putstr_blocking(uart, str);
                break;

                case 'x':
                num = va_arg(arglist, int);
                uart_putnum_blocking(uart, num, HEX);
                break;

                default:
                /* Do Nothing */
                break;
            }
        }
        formatstr++;
    }
    va_end(arglist);

}

char uart_getchar_blocking(uart_t uart)
{
    while ((uart_reg[uart]->SR & USART_SR_RXNE) == 0);
    return (char)(uart_reg[uart]->DR & 0xFF);
}

