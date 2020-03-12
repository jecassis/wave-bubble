/*
 * test.c
 *
 * Hardware test functions.
 *
 */

#ifdef TEST

#include "test.h"

#include <util/delay.h>

#include "pll.h"
#include "tuner.h"
#include "wavebubble.h"

/*
 * Milliseconds delay using microseconds delay function from AVR Libc.
 *
 * ms: Number of milliseconds to delay
 *
 */
void delay_ms(uint16_t ms) {
  volatile uint16_t i;

  while (ms != 0) {
    for (i = 0; i != 1000; ++i) {
      _delay_us(1);
    }
    --ms;
  }
}

/*
 * Test power/program indicator LED.
 *
 */
void test_led(void) {
  LEDPORT |= _BV(LED);
  delay_ms(125);
  LEDPORT &= ~_BV(LED);
  delay_ms(125);
}

/*
 * Test digital potentiometers by performing a sweep. The potentiometer will run contrary.
 *
 */
void test_resistors(void) {
  static uint8_t i = 0;

  set_resistor(BANDWADJ1_RES, i);
  set_resistor(BANDWADJ2_RES, 255 - i);
  delay_ms(50);
  ++i;
}

/*
 * Test VCO1 power switch by cycling VCO1 power every 2s.
 *
 */
void test_powerswitch1(void) {
  POWERCTL2_PORT &= ~_BV(POWERCTL2);
  POWERCTL1_PORT |= _BV(POWERCTL1);
  delay_ms(2000);

  POWERCTL1_PORT &= ~_BV(POWERCTL1);
  delay_ms(2000);
}

/*
 * Test VCO2 power switch by cycling VCO2 power every 2s.
 *
 */
void test_powerswitch2(void) {
  POWERCTL1_PORT &= ~_BV(POWERCTL1);
  POWERCTL2_PORT |= _BV(POWERCTL2);
  delay_ms(2000);

  POWERCTL2_PORT &= ~_BV(POWERCTL2);
  delay_ms(2000);
}

/*
 * Test variable DC generation via PWM. The PWM outputs will run contrary.
 *
 */
void test_dc(void) {
  static uint16_t i = 0;

  OCR1A = i & 0x3FF;
  OCR1B = 4095 - (i & 0x3FF);
  ++i;
  delay_ms(50);
}

/*
 * Test VCOs by tuning over the whole bandwidth.
 *
 */
void test_vcos(void) {
  uint16_t i;

  POWERCTL2_PORT |= _BV(POWERCTL2);
  POWERCTL1_PORT |= _BV(POWERCTL1);

  set_resistor(BANDWADJ1_RES, 0);
  set_resistor(BANDWADJ2_RES, 0);
  for (i = 0; i < 4094; ++i) {
    OCR1A = i;
    OCR1B = i;
    delay_ms(50);
  }

  OCR1A = OCR1B = 2048;
  for (i = 0; i < 254; ++i) {
    set_resistor(BANDWADJ1_RES, i);
    set_resistor(BANDWADJ2_RES, i);
    delay_ms(50);
  }
}

/*
 * Test PLL by oscillating the Ftest/LD pin (pin 10) at 5Hz (1/200ms).
 *
 */
void test_pll1(void) {
  uint32_t out;
  uint16_t R = 10 & LMX2433_R0_R3_R_MASK; // 1MHz reference clock

  out = (LMX2433_R0_MUX_BITS(LMX2433_MUX_RF_PLL_DIGITAL_LOCK_DETECT) << LMX2433_R0_R3_MUX_BIT_SHIFT) | (R << LMX2433_R0_R3_R_BIT_SHIFT);
  pll_tx(out, LMX2433_R0_RF_R_ADDRESS);

  in_char = 0;

  while (in_char == 0) {
    out = (LMX2433_R3_MUX_BITS(LMX2433_MUX_IF_PLL_ANALOG_LOCK_DETECT) << LMX2433_R0_R3_MUX_BIT_SHIFT) | (R << LMX2433_R0_R3_R_BIT_SHIFT);
    pll_tx(out, LMX2433_R3_IF_R_ADDRESS);
    delay_ms(100);

    out = (LMX2433_R3_MUX_BITS(LMX2433_MUX_RF_PLL_DIGITAL_LOCK_DETECT) << LMX2433_R0_R3_MUX_BIT_SHIFT) | (R << LMX2433_R0_R3_R_BIT_SHIFT);
    pll_tx(out, LMX2433_R3_IF_R_ADDRESS);
    delay_ms(100);
  }
}

/*
 * Test the RF stage of the PLL.
 *
 */
void test_pll2_rf(void) {
  pll_init();

  set_sawtooth_low();
  set_resistor(BANDWADJ1_RES, 0);

  pll_set_rf(2000, 8);
  POWERCTL1_PORT |= _BV(POWERCTL1);  // Turn on VCO1
  POWERCTL2_PORT &= ~_BV(POWERCTL2); // Turn off VCO2

  in_char = 0;

  while (in_char == 0) {
    ++OCR1A;
    delay_ms(100);
  }
}

/*
 * Test the IF stage of the PLL.
 *
 */
void test_pll2_if(void) {
  pll_init();

  set_sawtooth_low();
  set_resistor(BANDWADJ2_RES, 0);

  pll_set_if(1200, 8);
  POWERCTL2_PORT |= _BV(POWERCTL2);  // Turn on VCO2
  POWERCTL1_PORT &= ~_BV(POWERCTL1); // Turn off VCO1

  in_char = 0;

  while (in_char == 0) {
    ++OCR1B;
    delay_ms(50);
  }
}

#endif
