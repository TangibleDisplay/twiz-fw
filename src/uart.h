#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init(void);
int putchar(int c);
int getchar(void);
bool getchar_timeout(uint32_t timeout_ms, char *c);
void getline(int size, char *buf);

#endif
