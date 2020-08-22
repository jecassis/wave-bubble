/*
 * serial.h
 *
 * Serial header file.
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>

#define BAUD 19200 // Serial baud rate for terminal communication

extern void usart_init(void);
extern void usart_putc(char data);
extern void usart_puts_P(const char *s);
#ifdef DEBUG
extern void usart_puts(char *s);
extern void usart_flush(char *s);
#endif
extern char usart_getc(void);
extern uint16_t usart_get16(void);
#ifdef DEBUG
extern void putnum_uh(uint16_t n);
#endif
extern void putnum_ud(uint16_t n);
extern void print_div(void);

#endif
