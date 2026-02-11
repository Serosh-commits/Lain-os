#include "drivers/uart.h"
#include "kernel/riscv.h"
#include "lib/types.h"

#define RHR 0
#define THR 0
#define IER 1
#define FCR 2
#define ISR 2
#define LCR 3
#define LSR 5

#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE (1 << 5)

#define UART_REG(reg) (*(volatile uint8_t*)(UART0 + (reg)))

void uart_init() {
    UART_REG(IER) = 0x00;
    UART_REG(FCR) = 0x01;
    UART_REG(LCR) = 0x03;
    UART_REG(IER) = 0x00;
}

void uart_putc(int c) {
    while ((UART_REG(LSR) & LSR_TX_IDLE) == 0)
        ;
    UART_REG(THR) = c;
}

int uart_getc() {
    if (UART_REG(LSR) & LSR_RX_READY) {
        return UART_REG(RHR);
    }
    return -1;
}

void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}

void uart_putnum(uint64_t x, int base) {
    static char digits[] = "0123456789abcdef";
    char buf[65];
    int i = 0;
    
    if (x == 0) {
        uart_putc('0');
        return;
    }
    
    while (x != 0) {
        buf[i++] = digits[x % base];
        x /= base;
    }
    
    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

void uart_puthex(uint64_t x) {
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uart_putc("0123456789abcdef"[(x >> i) & 0xf]);
    }
}
