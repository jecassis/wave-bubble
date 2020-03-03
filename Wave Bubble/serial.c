/**
 * \file serial.c
 *
 * USART and serial I/O functions
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <ctype.h>
#include <string.h>
#include "main.h"
#include "serial.h"

/**
 * Init USART
 *
 * Initialize USART for communication with PC.
 *
*/
void usart_init(void) {
        // Setup USART
        UBRR0H = (((F_CPU/BAUDRATE)/16)-1)>>8;      // set baud rate
        UBRR0L = (((F_CPU/BAUDRATE)/16)-1);
        // enable Rx & Tx
        UCSR0B = _BV(RXCIE0)|_BV(RXEN0)|_BV(TXEN0);
        // config 8N1
        UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
}


/**
 * Send one byte to PC
 *
 * \param       data    Byte to send
 *
*/
void pc_putc(char data) {
        // wait for USART to become available
        while ( (UCSR0A & _BV(UDRE0)) != _BV(UDRE0));
        UDR0 = data;    // send character
}


/**
 * Send RAM string to PC
 *
 * \param       s       String to send, stored in RAM
 *
*/
void pc_puts(char *s) {
        while(*s) pc_putc(*s++);   // send string char by char
}


/**
 * Send flash string to PC
 *
 * \param       s       String to send, stored in flash, use PSTR macro
 *
*/
void pc_puts_P(const char *s) {
        // send string char by char
        while(pgm_read_byte(&*s)) pc_putc(pgm_read_byte(&*s++));
};

/**
 * Interrupt service routine to receive data from PC via USART0
 *
*/
ISR(USART_RX_vect) {
        uint8_t tmp = UDR0; // get received byte

        if(isalpha(tmp)) in_char = tmp;
}

/**
 * Print unsigned integer on terminal
 *
 * \param       n       Number to print on terminal
 *
*/
void putnum_ud(uint16_t n) {
        uint8_t cnt=0, flag=0;

        while (n >= 10000UL) { flag = 1; cnt++; n -= 10000UL; }
        if (flag) pc_putc('0'+cnt);
        cnt = 0;
        while (n >= 1000UL) { flag = 1; cnt++; n -= 1000UL; }
        if (flag) pc_putc('0'+cnt);
        cnt = 0;
        while (n >= 100UL) { flag = 1; cnt++; n -= 100UL; }
        if (flag) pc_putc('0'+cnt);
        cnt = 0;
        while (n >= 10UL) { flag = 1; cnt++; n -= 10UL; }
        if (flag) pc_putc('0'+cnt);
        cnt = 0;
        pc_putc('0'+n);
        return;
}
