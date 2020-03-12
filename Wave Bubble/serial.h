/*
 * serial.h
 *
 * Serial header file.
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>

#define BAUDRATE 19200 // Serial baud rate for terminal communication

#if !defined(F_CPU)
#error "No MCU crystal speed defined!"
#endif

extern void usart_init(void);
extern void pc_putc(char data);
extern void pc_puts(char *s);
extern void pc_puts_P(const char *s);
extern char pc_getc(void);
extern uint16_t pc_read16(void);
extern void putnum_uh(uint16_t n);
extern void putnum_ud(uint16_t n);
extern void print_div(void);

#endif
