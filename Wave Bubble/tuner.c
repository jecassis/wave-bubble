/*
 * tuner.c
 *
 * Tuning subsystem component functions.
 *
 */

#include "tuner.h"

#include <avr/cpufunc.h>

/*
 * Initialize variable DC generation via PWM from timer 1.
 *
 */
void init_pwm(void) {
  DDRB |= _BV(PB1);                                                          // Make OC1A an output
  DDRB |= _BV(PB2);                                                          // Make OC1B an output
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM12) | _BV(WGM11) | _BV(WGM10); // Fast PWM, 10-bit
  TCCR1B = _BV(CS10);
  OCR1A = 0x0; // Change this register's value to adjust OC1A PWM
  OCR1B = 0x0; // Change this register's value to adjust OC1B PWM
}

/*
 * Set NE555 low frequency mode (100Hz sawtooth).
 *
 */
void set_sawtooth_low(void) {
  // Set the pin to be a high input
  FREQSET_DDR &= ~_BV(FREQSET);
  FREQSET_PORT |= _BV(FREQSET);
}

/*
 * Set NE555 high frequency mode (20KHz sawtooth).
 *
 */
void set_sawtooth_high(void) {
  // Set the pin to be a high output
  FREQSET_DDR |= _BV(FREQSET);
  FREQSET_PORT |= _BV(FREQSET);
}

/*
 * Set digital variable resistor.
 *
 * Notes:
 * 1. DATA is clocked into the 10-bit shift register on the rising edge of CLK
 * 2. The shift register consists of a 2-bit ADDRESS field and a 8-bit DATA field
 * 3. The MSB of 2-bit ADDRESS is shifted in first
 *
 * rnum: Number of potentiometer, 0 or 1
 * rval: Resistor value to set, 0-255
 *
 */
void set_resistor(uint8_t rnum, uint8_t rval) {
  uint16_t reg_mask, data;

  data = rnum & AD8402_ADDRESS_SIZE_MASK;
#ifdef DEBUG
  if (data != AD8402_RDAC1_ADDRESS || data != AD8402_RDAC2_ADDRESS) {
    usart_puts_P(PSTR("Only 2 variable resistors on AD8402."));
    return;
  }
#endif
  data <<= AD8402_ADDRESS_BIT_SHIFT;
  data = (data | rval) & AD8402_REG_MASK;

  // CS low
  // t_CSS: CS Setup Time -- 10ns minimum
  SPICS_PORT &= ~_BV(SPICS);
  _NOP();

  for (reg_mask = AD8402_MSB_MASK; reg_mask != 0; reg_mask >>= 1) {
    // Set DATA high or low depending on masked value
    if (data & reg_mask) {
      SPISDO_PORT |= _BV(SPISDO);
    } else {
      SPISDO_PORT &= ~_BV(SPISDO);
    }

    // t_DS: DATA Setup Time -- 5ns minimum
    // Set CLK high to latch the DATA bit
    SPICLK_PORT |= _BV(SPICLK);

    // t_CH: Input Clock Pulse Width -- 10ns minimum
    // t_DH: Data Hold Time -- 5ns minimum
    // Set CLK low to clock in the next DATA bit
    _NOP();
    SPICLK_PORT &= ~_BV(SPICLK);

    // t_CL: CLK Pulse Width Low -- 10ns minimum
  }

  // t_CSH: CLK Fall to CS Rise Hold Time -- 0ns minimum
  // t_CS1: CS Rise to CLK Rise Setup -- 10ns minimum
  // Pull CS line high to load the DAC register
  _NOP();
  SPICS_PORT |= _BV(SPICS);

  // t_CSW: CS High Pulse Width -- 10ns minimum
}
