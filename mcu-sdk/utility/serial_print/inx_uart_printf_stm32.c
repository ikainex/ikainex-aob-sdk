#include "inx_uart_printf_stm32.h"
#include <string.h>

#ifndef INX_UART_PRINTF_USE_LPUART
#define INX_UART_PRINTF_USE_LPUART 0
#endif

#if INX_UART_PRINTF_USE_LPUART
#include "stm32l5xx_ll_usart.h"
#endif

static UART_HandleTypeDef *g_uart = NULL;

void inx_uart_printf_init(UART_HandleTypeDef *huart)
{
    g_uart = huart;
}

static void inx_uart_printf_write(const uint8_t *data, uint16_t size)
{
#if INX_UART_PRINTF_USE_LPUART
    uint16_t index;

    for (index = 0; index < size; index++)
    {
        while (!LL_USART_IsActiveFlag_TXE(g_uart->Instance))
        {
        }

        LL_USART_TransmitData8(g_uart->Instance, data[index]);
    }

    while (!LL_USART_IsActiveFlag_TC(g_uart->Instance))
    {
    }
#else
    HAL_UART_Transmit(g_uart, (uint8_t *)data, size, HAL_MAX_DELAY);
#endif
}

void inx_uart_printf(const char *fmt, ...)
{
    char buffer[256];

    va_list args;

    if (g_uart == NULL)
    {
        return;
    }

    va_start(args, fmt);

    vsnprintf(buffer,
              sizeof(buffer),
              fmt,
              args);

    va_end(args);

    inx_uart_printf_write((const uint8_t *)buffer, (uint16_t)strlen(buffer));
}

/*------------------------------------------------------------------
 * printf() Redirection
 *-----------------------------------------------------------------*/

#ifdef __GNUC__

int __io_putchar(int ch)

#else

int fputc(int ch, FILE *f)

#endif
{
    if (g_uart != NULL)
    {
        uint8_t character = (uint8_t)ch;
        inx_uart_printf_write(&character, 1);
    }

    return ch;
}
