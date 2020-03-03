/**
 * \file main.c
 *
 * \mainpage Wave Bubble 2010 Hardware Test
 *
 * \version v1.0a \n
 *
 * Firmware to test the Wave Bubble 2010 hardware after assembly \n\n
 *
 * Copyright (c) 2011, Mictronics
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, \n
 * are permitted provided that the following conditions are met: \n
 *
 * Redistributions of source code must retain the above copyright notice, \n
 * this list of conditions and the following disclaimer. \n
 * Redistributions in binary form must reproduce the above copyright notice, \n
 * this list of conditions and the following disclaimer in the documentation and/or \n
 * other materials provided with the distribution. \n
 * Neither the name Mictronics nor the names of its contributors may be used \n
 * to endorse or promote products derived from this software without specific prior \n
 * written permission. \n\n
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND \n
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \n
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE \n
 * ARE DISCLAIMED. \n
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, \n
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES \n
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; \n
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND \n
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT \n
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, \n
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. \n\n
 *
 * These documentation pages are generated regularly from the current source code.\n
 *
 * For questions, bug reports or problems use the forum at <a href="http://forums.adafruit.com/viewforum.php?f=16">http://forums.adafruit.com/viewforum.php?f=16</a>
 *
 * \author  Mictronics
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "test.h"
#include "pll.h"
#include "main.h"

volatile char in_char = 0;

/**
 * Init variable DC generation via PWM from timer 1
 */
void init_pwm(void) {
  DDRB |= _BV(PB1);  // make OCR1A it an output
  DDRB |= _BV(PB2);  // make OCR1B it an output

#if 0
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM12) | _BV(WGM10); // 8bit FastPWM
#else
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM12) | _BV(WGM11) | _BV(WGM10); // 10bit FastPWM
#endif
  TCCR1B = _BV(CS10);
  OCR1A = 0x0;  // change this value to adjust PWM.
  OCR1B = 0x0;  // change this value to adjust PWM.
}

/**
 * The main function.
 *
 * \return Zero
 *
 */
int main(void) {

  // Power ON switch pin output and force HIGH to keep device running
  POWERON_PORT |= _BV(POWERON);
  POWERON_DDR |= _BV(POWERON);

  usart_init(); // Init serial communication

  DDRD |= _BV(PD1);     // USART TxD0 output

  // LED pin output and LED off
  LEDDDR |= _BV(LED);
  LEDPORT &=~ _BV(LED);

  // Setup port and pins for software SPI operation to control digital potentiometer
  SPICS_DDR |= _BV(SPICS);
  SPIDO_DDR |= _BV(SPIDO);
  SPICLK_DDR |= _BV(SPICLK);

  SPICS_PORT |= _BV(SPICS);
  SPICLK_PORT &= ~_BV(SPICLK);

  // Setup VCO power control port and pins
  POWERCTL1_DDR |= _BV(POWERCTL1);
  POWERCTL2_DDR |= _BV(POWERCTL2);
  POWERCTL1_PORT &= ~_BV(POWERCTL1); // turn off VCO1+gain stage
  POWERCTL2_PORT &= ~_BV(POWERCTL2); // turn off VCO2+gain stage

  init_pwm(); // Init tuning voltage generator

  pll_init(); // Init PLL control

  // Set digital potentiometer to minimum
  set_resistor(BANDWADJ1_RES, 0);
  set_resistor(BANDWADJ2_RES, 0);

  set_sawtooth_low(); // Set NE555 mode

  pc_puts_P(PSTR("Wave Bubble 2010\nHardware Test\nFW: " __DATE__" / " __TIME__"\n\n"));

  pc_puts_P(PSTR("Serial connection test passed if you can read this.\n\n"));

  pc_puts_P(PSTR("Select a test:\n"));
  pc_puts_P(PSTR("a - Power/Program LED test\n")); // TP1
  pc_puts_P(PSTR("b - NE555 low frequency mode\n")); // TP2
  pc_puts_P(PSTR("c - NE555 high frequency mode\n")); // TP2
  pc_puts_P(PSTR("d - Digital potentiometer sweep\n")); // TP3 + TP4
  pc_puts_P(PSTR("e - Set digital potentiometer minimum\n"));
  pc_puts_P(PSTR("f - Set digital potentiometer maximum\n"));
  pc_puts_P(PSTR("g - Power switch VCO1 (HI) test\n")); // TP5
  pc_puts_P(PSTR("h - Power switch VCO2 (LO) test\n")); // TP6
  pc_puts_P(PSTR("i - PWM tuning voltage generator sweep\n")); // TP7 + TP8
  pc_puts_P(PSTR("j - Set PWM tuning voltage minimum\n"));
  pc_puts_P(PSTR("k - Set PWM tuning voltage maximum\n"));
  pc_puts_P(PSTR("l - VCO sweep test\n")); // TP9 + TP10
  pc_puts_P(PSTR("m - VCO1 (HI) power toggle\n")); // TP6
  pc_puts_P(PSTR("n - VCO2 (LO) power toggle\n")); // TP7
  pc_puts_P(PSTR("o - PLL test 1 - DATA signal 5Hz\n"));
  pc_puts_P(PSTR("p - PLL test 2 - RF stage\n"));
  pc_puts_P(PSTR("q - PLL test 3 - IF stage\n"));
  pc_puts_P(PSTR("r - Increase PWM tuning voltage by 1\n"));
  pc_puts_P(PSTR("s - Decrease PWM tuning voltage by 1\n"));

  pc_puts_P(PSTR("\nz - Power OFF\n"));

  sei();

  uint16_t ocr_val = 0;

  for(;;) {

    switch(in_char) {

    case 'a':
      test_led();
      break;

    case 'b':
      set_sawtooth_low();
      break;

    case 'c':
      set_sawtooth_high();
      break;

    case 'd':
      test_resistors();
      break;

    case 'e':
      set_resistor(BANDWADJ1_RES, 0);
      set_resistor(BANDWADJ2_RES, 0);
      break;

    case 'f':
      set_resistor(BANDWADJ1_RES, 255);
      set_resistor(BANDWADJ2_RES, 255);
      break;

    case 'g':
      test_powerswitch1();
      break;

    case 'h':
      test_powerswitch2();
      break;

    case 'i':
      test_dc();
      break;

    case 'j':
      OCR1A = 0;
      OCR1B = 0;
      ocr_val = 0;
      break;

    case 'k':
      OCR1A = 4095;
      OCR1B = 4095;
      ocr_val = 4095;
      break;

    case 'l':
      test_vcos();
      break;

    case 'm':
      OCR1A = 0;
      set_resistor(BANDWADJ1_RES, 0);
      POWERCTL2_PORT &= ~_BV(POWERCTL2);
      POWERCTL1_PORT ^= _BV(POWERCTL1);
      in_char = 0;
      break;

    case 'n':
      OCR1B = 0;
      set_resistor(BANDWADJ2_RES, 0);
      POWERCTL1_PORT &= ~_BV(POWERCTL1);
      POWERCTL2_PORT ^= _BV(POWERCTL2);
      in_char = 0;
      break;

    case 'o':
      test_pll1();
      break;

    case 'p':
      test_pll2_rf();
      break;

    case 'q':
      test_pll2_if();
      break;

    case 'r':
      if(ocr_val < 4096) {
        ++ocr_val;
        OCR1A = OCR1B = ocr_val;
      }
      in_char = 0;
      break;

    case 's':
      if(ocr_val > 0) {
        --ocr_val;
        OCR1A = OCR1B = ocr_val;
      }
      in_char = 0;
      break;

    case 'z':
      #ifdef HW_REV_A
        POWERON_PORT |= _BV(POWERON);
      #else
        POWERON_PORT &=~ _BV(POWERON);
      #endif
      break;

    default:
      break;
    } // end switch(state)
  } // end for(;;)

  return 0;
}
