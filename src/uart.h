#ifndef UART_H
#define UART_H

void uart_init(void);

#ifdef UART_IRQ
#define putchar(c) do {while (app_uart_put(c) != NRF_SUCCESS);} while(0)
#else
#define putchar simple_uart_put
#endif

#endif
