#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

typedef union uart_flags {
    struct {
        uint8_t clear_to_send : 1;
        uint8_t data_set_ready : 1;
        uint8_t data_carrier_detected : 1;
        uint8_t busy : 1;
        uint8_t recieve_queue_empty : 1;
        uint8_t transmit_queue_full : 1;
        uint8_t recieve_queue_full : 1;
        uint8_t transmit_queue_empty : 1;
        uint8_t ring_indicator : 1;
        uint32_t padding : 23;
    };
    uint32_t as_int;
} uart_flags_t;

typedef union uart_control {
    struct {
        uint8_t uart_enabled : 1;
        uint8_t sir_enabled : 1;
        uint8_t sir_low_power_mode : 1;
        uint8_t reserved : 4;
        uint8_t loop_back_enabled : 1;
        uint8_t transmit_enabled : 1;
        uint8_t receive_enabled : 1;
        uint8_t data_transmit_ready : 1;
        uint8_t request_to_send : 1;
        uint8_t out1 : 1;
        uint8_t out2 : 1;
        uint8_t rts_hardware_flow_control_enabled : 1;
        uint8_t cts_hardware_flow_control_enabled : 1;
        uint16_t padding;
    };
    uint32_t as_int;
} uart_control_t;

typedef enum {
// The GPIO registers base address.
#ifdef PI_1
    GPIO_BASE = 0x20200000,
    // The base address for UART.
    UART0_BASE = 0x20201000,
#else
    GPIO_BASE = 0x3F200000,
    UART0_BASE = 0x3F201000,
#endif

    // The offsets for reach register.
    // controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),

    // controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The offsets for reach register for the UART.
    UART0_DR = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR = (UART0_BASE + 0x18),
    UART0_ILPR = (UART0_BASE + 0x20),
    UART0_IBRD = (UART0_BASE + 0x24),
    UART0_FBRD = (UART0_BASE + 0x28),
    UART0_LCRH = (UART0_BASE + 0x2C),
    UART0_CR = (UART0_BASE + 0x30),
    UART0_IFLS = (UART0_BASE + 0x34),
    UART0_IMSC = (UART0_BASE + 0x38),
    UART0_RIS = (UART0_BASE + 0x3C),
    UART0_MIS = (UART0_BASE + 0x40),
    UART0_ICR = (UART0_BASE + 0x44),
    UART0_DMACR = (UART0_BASE + 0x48),
    UART0_ITCR = (UART0_BASE + 0x80),
    UART0_ITIP = (UART0_BASE + 0x84),
    UART0_ITOP = (UART0_BASE + 0x88),
    UART0_TDR = (UART0_BASE + 0x8C),
} UART_ADDRS;

void mmio_write(uint32_t reg, uint32_t data);

uint32_t mmio_read(uint32_t reg);

void delay(int32_t count);

void uart_init();

void uart_putc(unsigned char c);

unsigned char uart_getc();

void uart_puts(const char *str);

void uart_gets(char *buffer, int buff_len);

uart_flags_t read_flags(void);

#endif
