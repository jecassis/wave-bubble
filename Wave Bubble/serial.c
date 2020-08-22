/*
 * serial.c
 *
 * USART and serial I/O functions.
 *
 */

#include "serial.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#include <string.h>
#include <util/setbaud.h>

#include "wavebubble.h"

/* Initialize USART for communication with PC. */
void usart_init(void) {
  // Set baud rate
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  // Enable receiver and transmitter
#ifdef TEST
  UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
#else
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#endif

  // Set frame format: 8N1
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

#ifdef TEST
/* Interrupt service routine to receive data from PC via USART0. */
ISR(USART_RX_vect) {
  uint8_t tmp = UDR0; // get received byte

  if (isalpha(tmp)) {
    in_char = tmp;
  }
}
#endif

/* Send one byte on USART. */
void usart_putc(char data) {
  // Wait for empty transmit buffer
  loop_until_bit_is_set(UCSR0A, UDRE0);

  // Put data into buffer, sends the data
  UDR0 = data;
}

/* Send string stored in flash (i.e. using PSTR macro) on USART. */
void usart_puts_P(const char *s) {
  // Send string char by char
  while (pgm_read_byte(&*s)) {
    usart_putc(pgm_read_byte(&*s++));
  }
}

#ifdef DEBUG
/* Send string stored in RAM on USART. */
void usart_puts(char *s) {
  while (*s) {
    usart_putc(*s++); // Send string char by char
  }
}

/* Flush USART buffer. */
void usart_flush(void) {
  unsigned char dummy;
  while (bit_is_set(UCSR0A, RXC0)) {
    dummy = UDR0;
  }
}
#endif

/* Get one byte from USART. */
char usart_getc(void) {
  // Wait for data to be received
  loop_until_bit_is_set(UCSR0A, RXC0);

  // Get and return received data from buffer
  return UDR0;
}

/* Get unisgned integer from USART. */
uint16_t usart_get16(void) {
  uint8_t c;
  uint16_t t = 0;
  while ((c = usart_getc()) != '\n') {
    if (c == '\r') {
      break;
    }
    if ((c > '9') || (c < '0')) {
      continue;
    }
    usart_putc(c);
    t *= 10;
    t += c - '0';
  }
  usart_putc(c);

  return t;
}

#ifdef DEBUG
/* Send hexadecimal-equivalent character byte on USART. */
void usart_puth(uint8_t hex) {
  hex &= 0xF;
  usart_puts("0x");
  if (hex < 10) {
    usart_putc('0' + hex);
  } else {
    usart_putc('A' - 10 + hex);
  }
}

/* Print unsigned hexadecimal on terminal. */
void putnum_uh(uint16_t n) {
  if (n >> 12) {
    usart_puth(n >> 12);
  }

  if (n >> 8) {
    usart_puth(n >> 8);
  }

  if (n >> 4) {
    usart_puth(n >> 4);
  }

  usart_puth(n);
}
#endif

/* Print unsigned integer on terminal. */
void putnum_ud(uint16_t n) {
  uint8_t cnt = 0, flag = 0;

  while (n >= 10000UL) {
    flag = 1;
    ++cnt;
    n -= 10000UL;
  }
  if (flag) {
    usart_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 1000UL) {
    flag = 1;
    ++cnt;
    n -= 1000UL;
  }
  if (flag) {
    usart_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 100UL) {
    flag = 1;
    ++cnt;
    n -= 100UL;
  }
  if (flag) {
    usart_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 10UL) {
    flag = 1;
    ++cnt;
    n -= 10UL;
  }
  if (flag) {
    usart_putc('0' + cnt);
  }

  cnt = 0;
  usart_putc('0' + n);
}

/* Print divider on terminal. */
void print_div(void) {
  usart_puts_P(PSTR("---------------------------------\n"));
}
