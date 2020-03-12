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

#include "wavebubble.h"

/*
 * Initialize USART for communication with PC.
 *
 */
void usart_init(void) {
  // Set baud rate
  UBRR0H = (((F_CPU / BAUDRATE) / 16) - 1) >> 8;
  UBRR0L = (((F_CPU / BAUDRATE) / 16) - 1);

  // Enable Rx and Tx
#ifdef TEST
  UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
#else
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#endif

  // Configure 8N1
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

#ifdef TEST
/*
 * Interrupt service routine to receive data from PC via USART0.
 *
 */
ISR(USART_RX_vect) {
  uint8_t tmp = UDR0; // get received byte

  if (isalpha(tmp)) {
    in_char = tmp;
  }
}
#endif

/*
 * Send one byte to PC.
 *
 * data: Byte to send
 *
 */
void pc_putc(char data) {
  // Wait for USART to become available
  loop_until_bit_is_set(UCSR0A, UDRE0);

  UDR0 = data; // Send character
}

/*
 * Send RAM string to PC.
 *
 * s: String to send, stored in RAM
 *
 */
void pc_puts(char *s) {
  while (*s) {
    pc_putc(*s++); // Send string char by char
  }
}

/*
 * Send flash string to PC.
 *
 * s: String to send, stored in flash, using PSTR macro
 *
 */
void pc_puts_P(const char *s) {
  // Send string char by char
  while (pgm_read_byte(&*s)) {
    pc_putc(pgm_read_byte(&*s++));
  }
}

/*
 * Get one byte from PC.
 *
 * Returns received byte
 *
*/
char pc_getc(void) {
  //  Wait for USART receive complete flag
  loop_until_bit_is_set(UCSR0A, RXC0);

  return UDR0;
}

/*
 * Get unisgned integer from PC.
 *
 * Returns received number
 *
*/
uint16_t pc_read16(void) {
  uint8_t c;
  uint16_t t = 0;
  while ((c = pc_getc()) != '\n') {
    if (c == '\r') {
      break;
    }
    if ((c > '9') || (c < '0')) {
      continue;
    }
    pc_putc(c);
    t *= 10;
    t += c - '0';
  }
  pc_putc(c);

  return t;
}

/*
 * Send hexadecimal-equivalent character byte to PC.
 *
 * hex: Digit to send
 *
 */
static void pc_puth(uint8_t hex) {
  hex &= 0xF;
  if (hex < 10) {
    pc_putc('0' + hex);
  } else {
    pc_putc('A' - 10 + hex);
  }
}

/*
 * Print unsigned hexadecimal on terminal.
 *
 * n: Number to print on terminal
 *
 */
void putnum_uh(uint16_t n) {
  if (n >> 12) {
    pc_puth(n >> 12);
  }

  if (n >> 8) {
    pc_puth(n >> 8);
  }

  if (n >> 4) {
    pc_puth(n >> 4);
  }

  pc_puth(n);
}

/*
 * Print unsigned integer on terminal.
 *
 * n: Number to print on terminal
 *
 */
void putnum_ud(uint16_t n) {
  uint8_t cnt = 0, flag = 0;

  while (n >= 10000UL) {
    flag = 1;
    ++cnt;
    n -= 10000UL;
  }
  if (flag) {
    pc_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 1000UL) {
    flag = 1;
    ++cnt;
    n -= 1000UL;
  }
  if (flag) {
    pc_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 100UL) {
    flag = 1;
    ++cnt;
    n -= 100UL;
  }
  if (flag) {
    pc_putc('0' + cnt);
  }

  cnt = 0;
  while (n >= 10UL) {
    flag = 1;
    ++cnt;
    n -= 10UL;
  }
  if (flag) {
    pc_putc('0' + cnt);
  }

  cnt = 0;
  pc_putc('0' + n);
}

/*
 * Print divider on terminal
 *
 */
void print_div(void) {
  pc_puts_P(PSTR("---------------------------------\n"));
}
