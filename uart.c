/**
 * uart.c contains some of the core
 */

#include "lib/uart.h"
#include <stddef.h>
#include <stdint.h>

// Memory-Mapped I/O output
void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t *)reg = data;
}

// Memory-Mapped I/O input
uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t *)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
// TODO can probably get rid of this or rethink implementation
void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                 : "=r"(count)
                 : [count] "0"(count)
                 : "cc");
}

void uart_init() {
    // Disable UART0.
    mmio_write(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    mmio_write(UART0_ICR, 0x7FF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.

    // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
    mmio_write(UART0_IBRD, 1);
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    mmio_write(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Mask all interrupts.
    mmio_write(UART0_IMSC,
               (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) |
                   (1 << 9) | (1 << 10));

    uart_control_t uart_control;

    // Enable UART0, receive & transfer part of UART.
    // mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
    uart_control.uart_enabled = 1;
    uart_control.transmit_enabled = 1;
    uart_control.receive_enabled = 1;
    mmio_write(UART0_CR, uart_control.as_int);
}

void uart_putc(unsigned char c) {
    uart_flags_t flags;
    // Wait for UART to become ready to transmit.

    do {
        flags = read_flags();
    } while (flags.transmit_queue_full);
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc() {
    // Wait for UART to have received something.
    uart_flags_t flags;
    do {
        flags = read_flags();
    } while (flags.recieve_queue_empty);

    return mmio_read(UART0_DR);
}

void uart_puts(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        uart_putc((unsigned char)str[i]);
}

void uart_gets(char *buffer, int buff_len) {
    int i;
    char c;

    // leaveing a empty spot for the null char in buff
    for (i = 0; (c = uart_getc()) != '\r' && buff_len > 1; i++, buff_len--) {
        uart_putc(c);
        buffer[i] = c;
    }
    uart_putc('\n');
    if (c == '\n') {
        buffer[i] = '\0';
    } else {
        buffer[buff_len - 1] = '\0';
    }
}

uart_flags_t read_flags(void) {
    uart_flags_t flags;
    flags.as_int = mmio_read(UART0_FR);
    return flags;
}
