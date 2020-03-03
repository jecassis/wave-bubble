/**
 * \file test.c
 *
 * Hardware test functions
 *
 */
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"
#include "pll.h"

/**
 * Milliseconds delay function
 *
 * Used microseconds delay function from avrlibc
 *
 * \param       ms      Number of Milliseconds to delay
 *
 */
void delay_ms(uint16_t ms) {
  volatile uint16_t i;
  while (ms != 0) {
    for (i=0; i != 1000; i++) _delay_us(1);
    ms--;
  }
}

/**
 * Test power/program indicator LED
 *
 */
void test_led(void) {
  LEDPORT |= _BV(LED);
  delay_ms(125);
  LEDPORT &= ~_BV(LED);
  delay_ms(125);
}

/**
 * Set NE555 high frequency mode
 *
 * Enable 20KHz sawtooth
 *
 */
void set_sawtooth_high(void) {
  // set the pin to be a high output
  FREQSET_DDR |= _BV(FREQSET);
  FREQSET_PORT |= _BV(FREQSET);
}

/**
 * Set NE555 low frequency mode
 *
 * Enable 100Hz sawtooth
 *
 */
void set_sawtooth_low(void) {
  // set the pin to be a high input
  FREQSET_DDR &= ~_BV(FREQSET);
  FREQSET_PORT |= _BV(FREQSET);
}

/**
 * Set digital potentiometer
 *
 * \param       rnum    Number of potentiometer, 0 or 1
 * \param       rval    Resistor value to set, 0 - 255
 *
 */
void set_resistor(uint8_t rnum, uint8_t rval) {
  uint16_t d;

  d = rnum;
  d <<= 8;
  d |= rval;

  SPICS_PORT &= ~_BV(SPICS);
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");

  for (rnum=0; rnum < 10; rnum++) {
    if (d & 0x200) {
      SPIDO_PORT |= _BV(SPIDO);
    } else {
      SPIDO_PORT &= ~_BV(SPIDO);
    }
    SPICLK_PORT |= _BV(SPICLK);
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    SPICLK_PORT &= ~_BV(SPICLK);
    d <<= 1;
  }
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  SPICS_PORT |= _BV(SPICS);
}

/**
 * Test digital potentiometers
 *
 * Perform potentiometer sweep.
 * Potentiometer will run contrary.
 *
 */
void test_resistors(void) {
  static uint8_t i = 0;

  set_resistor(BANDWADJ1_RES, i);
  set_resistor(BANDWADJ2_RES, 255-i);
  delay_ms(50);
  i++;
}

/**
 * Test VCO1 power switch
 *
 * Cycle VCO1 power every 2s.
 *
 */
void test_powerswitch1(void) {
  POWERCTL2_PORT &= ~_BV(POWERCTL2);
  POWERCTL1_PORT |= _BV(POWERCTL1);
  delay_ms(2000);

  POWERCTL1_PORT &= ~_BV(POWERCTL1);
  delay_ms(2000);
}

/**
 * Test VCO2 power switch
 *
 * Cycle VCO2 power every 2s.
 *
 */
void test_powerswitch2(void) {
  POWERCTL1_PORT &= ~_BV(POWERCTL1);
  POWERCTL2_PORT |= _BV(POWERCTL2);
  delay_ms(2000);

  POWERCTL2_PORT &= ~_BV(POWERCTL2);
  delay_ms(2000);
}

/**
 * Test variable DC generation via PWM
 *
 * PWM outputs will run contrary.
 *
 */
void test_dc(void) {
  static uint16_t i = 0;
  OCR1A = i&0x3FF;
  OCR1B = 4095-(i&0x3FF);
  i++;
  delay_ms(50);
}

/**
 * Test VCOs
 *
 * VCOs will be tuned over the whole bandwidth.
 *
 */
void test_vcos(void) {
  uint16_t i;
  POWERCTL2_PORT |= _BV(POWERCTL2);
  POWERCTL1_PORT |= _BV(POWERCTL1);

  set_resistor(BANDWADJ1_RES, 0);
  set_resistor(BANDWADJ2_RES, 0);
  for (i = 0; i< 4094; i++) {
          OCR1A = i;
          OCR1B = i;
          delay_ms(50);
  }

  OCR1A = OCR1B = 2048;
  for (i = 0; i< 254; i++) {
          set_resistor(BANDWADJ1_RES, i);
          set_resistor(BANDWADJ2_RES, i);
          delay_ms(50);
  }
}

// oscillates pin #10 at 5 Hz
/**
 * Test PLL
 *
 * Oscillate PLL DATA pin at 5 Hz.
 *
 */
void test_pll1(void) {
  uint32_t out;

  out = 2; out <<= 19; out |= (10&0x7FFF);
  pll_tx(out, 0x3); // no otherbits set: defaults
  delay_ms(100);
  out = 1; out <<= 19; out |= (10&0x7FFF);
  pll_tx(out, 0x3); // no otherbits set: defaults
  delay_ms(100);
}

/**
 * Test PLL
 *
 * Test RF stage of PLL.
 *
 */
void test_pll2_rf(void) {
  pll_init();

  set_sawtooth_low();
  set_resistor(BANDWADJ1_RES, 0);

  pll_set_rf(2000, 8);
  POWERCTL1_PORT |= _BV(POWERCTL1); // Turn ON VCO1
  POWERCTL2_PORT &=~ _BV(POWERCTL2); // TURN OFF VCO2

  in_char = 0;

  while(in_char == 0) {
      OCR1A++;
      delay_ms(100);
  }
}

/**
 * Test PLL
 *
 * Test IF stage of PLL.
 *
 */
void test_pll2_if(void) {
  pll_init();

  set_sawtooth_low();
  set_resistor(BANDWADJ2_RES, 0);

  pll_set_if(1200, 8);
  POWERCTL2_PORT |= _BV(POWERCTL2); // Turn ON VCO2
  POWERCTL1_PORT &=~ _BV(POWERCTL1); // Turn OFF VCO1

  in_char = 0;

  while(in_char == 0) {
    OCR1B++;
    delay_ms(50);
  }
}
