#ifndef UART_H
#define UART_H

#include "lib/types.h"

void uart_init();
void uart_putc(int c);
int uart_getc();
void uart_puts(const char* s);
void uart_putnum(uint64_t x, int base);
void uart_puthex(uint64_t x);

#endif
