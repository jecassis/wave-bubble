/*
 * pll.c
 *
 * PLL driver functions.
 *
 */

#include "pll.h"

#include <avr/pgmspace.h>

#include "serial.h"
#ifdef TEST
#include "test.h"
#endif
#include "tuner.h"
#include "wavebubble.h"

/*
 * Send data to a specific PLL address.
 *
 * Notes:
 * 1. DATA is clocked into the 24-bit shift register on the rising edge of CLK
 * 2. The shift register consists of a 21-bit DATA field and a 3-bit ADDRESS field
 * 3. The MSB of DATA is shifted in first followed
 *
 * data: Data to send
 * addr: PLL register address
 *
 */
void pll_tx(uint32_t data, uint8_t addr) {
  uint32_t reg_mask;

#ifdef DEBUG
  if (addr > LMX2433_R5_IF_TOC_ADDRESS) {
    pc_puts_P(PSTR("Register does not exist."));
    return;
  }
#endif

  data = (data | (addr & LMX2433_ADDRESS_MASK)) & LMX2433_REG_MASK;

  // Bring LE low (clock in data while LE is low)
  PLLLE_PORT &= ~_BV(PLLLE);

  // Start out with the CLK low
  PLLCLK_PORT &= ~_BV(PLLCLK);

  for (reg_mask = LMX2433_MSB_MASK; reg_mask != 0; reg_mask >>= 1) {
    // Set DATA high or low depending on masked value
    if (data & reg_mask) {
      PLLDATA_PORT |= _BV(PLLDATA);
    } else {
      PLLDATA_PORT &= ~_BV(PLLDATA);
    }
    nop();

    // t_CS: DATA to CLK Setup Time -- 50ns minimum
    // Set CLK high to latch the DATA bit
    PLLCLK_PORT |= _BV(PLLCLK);
    nop();

    // t_CWH: CLK Pulse Width High -- 50ns minimum
    // t_CH: DATA to CLK Hold Time -- 10ns minimum
    // Set CLK low to clock in the next DATA bit
    PLLCLK_PORT &= ~_BV(PLLCLK);

    // t_CWL: CLK Pulse Width Low -- 50ns minimum
    // t_ES: CLK to LE Setup Time -- 50ns minimum
  }

  // Pull LE line high to latch DATA into the register
  // t_EW: LE Pulse Width -- 50ns minimum
  PLLLE_PORT |= _BV(PLLLE);
  nop();
  PLLLE_PORT &= ~_BV(PLLLE);
  PLLDATA_PORT &= ~_BV(PLLDATA);
}

/*
 * Initialize PLL.
 *
 */
void pll_init(void) {
  uint32_t out;
  uint16_t R = 10 & LMX2433_R0_R3_R_MASK; // 1MHz reference clock
  uint32_t disable = 0;

  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2);

  PLLCLK_DDR |= _BV(PLLCLK);
  PLLDATA_DDR |= _BV(PLLDATA);
  PLLLE_DDR |= _BV(PLLLE);
  PLL_IFIN_DDR &= ~_BV(PLL_IFIN);
  PLL_RFIN_DDR &= ~_BV(PLL_RFIN);
  PLL_IFIN_PORT &= ~_BV(PLL_IFIN);
  PLL_RFIN_PORT &= ~_BV(PLL_RFIN);

  pll_tx(disable, LMX2433_R2_RF_TOC_ADDRESS); // Disable RF Synthesizer Time Out Counter (TOC), fastlock, and set FLoutRF pin to general purpose and high impedance
  pll_tx(disable, LMX2433_R5_IF_TOC_ADDRESS); // Disable IF Synthesizer Time Out Counter (TOC), fastlock, and set OSCout/FLoutIF pin to general purpose and high impedance

  out = (LMX2433_R3_MUX_BITS(LMX2433_MUX_RF_N_DIV_BY_2) << LMX2433_R0_R3_MUX_BIT_SHIFT) | (R << LMX2433_R0_R3_R_BIT_SHIFT);
  pll_tx(out, LMX2433_R3_IF_R_ADDRESS);
  out = (LMX2433_R0_MUX_BITS(LMX2433_MUX_RF_N_DIV_BY_2) << LMX2433_R0_R3_MUX_BIT_SHIFT) | (R << LMX2433_R0_R3_R_BIT_SHIFT);
  pll_tx(out, LMX2433_R0_RF_R_ADDRESS);
}

/*
 * Set PLL frequency and prescaler.
 *
 * rf_freq:   RF frequency to set
 * P:         PLL prescaler
 * reg:       PLL stage to use, RF or IF
 *
 */
void pll_set_freq(uint16_t rf_freq, uint8_t P, uint8_t reg) {
  uint8_t prescaler;
  uint16_t B_mask, B, A, N;
  uint32_t out = 0;

#ifdef DEBUG
  if (((reg != LMX2433_R1_RF_N_ADDRESS) && (reg != LMX2433_R4_IF_N_ADDRESS)) ||
      ((P != LMX2433_PRESCALER_8_9) && (P != LMX2433_PRESCALER_16_17))) {
    pc_puts_P(PSTR("Can only write to R1 (RF) and R4 (IF) and use 8/9 and 16/17 as prescalers."));
    return;
  }
#endif

  // fCOMP: RF or IF phase detector comparison frequency (1MHz)
  // Fin: RF or IF input frequency (Fin = N * fCOMP)
  // A: RF_A or IF_A counter value
  // B: RF_B or IF_B counter value
  // P: Preset modulus of the dual modulus prescaler (P = 8 or 16)
  // N = Feedback divider frequency, therefore:
  // RF: RF_N = (RF_B * RF_P) + RF_A
  // IF: IF_N = (IF_B * IF_P) + IF_A
  N = rf_freq; // fcomp = 1MHz
  B = N / P;
  // Divide ratios less than 3 are prohibited
  if (B < 3) {
    B = 3;
  }
  A = N % P;

  pc_puts_P(PSTR("Set PLL for RF frequency "));
  putnum_ud(N);
  pc_puts_P(PSTR("MHz & prescaler "));
  putnum_ud(P);
  pc_puts_P(PSTR(": B="));
  putnum_ud(B);
  pc_puts_P(PSTR(" A="));
  putnum_ud(A);
  pc_putc('\n');

  // The R1/R4 registers contains the RF_A/IF_A, RF_B/IF_B, RF_P/IF_P, and RF_PD/IF_PD control words;
  // the RF_A/IF_A and RF_B/IF_B control words are used to set up the programmable feedback divider
  prescaler = (P == LMX2433_PRESCALER_16_17) ? LMX2433_R1_R4_SYNTH_PRESCALE_SELECT_16_17 : LMX2433_R1_R4_SYNTH_PRESCALE_SELECT_8_9;
  B_mask = (reg == LMX2433_R1_RF_N_ADDRESS) ? LMX2433_R1_B_MASK : LMX2433_R4_B_MASK;
  out = ((uint32_t)(A & LMX2433_R1_R4_A_MASK) << LMX2433_R1_R4_A_BIT_SHIFT) |
        ((uint32_t)(B & B_mask) << LMX2433_R1_R4_B_BIT_SHIFT) |
        ((uint32_t)prescaler << LMX2433_R1_R4_SYNTH_PRESCALE_BIT_SHIFT) |
        (LMX2433_R1_R4_SYNTH_PD_PLL_ACTIVE << LMX2433_R1_R4_SYNTH_PD_BIT_SHIFT);
  pll_tx(out, reg);
}

/*
 * Tune PLL RF stage (VCO1) by finding a PWM value for a specific frequency.
 *
 * freq: Frequency to tune
 * Returns OCR1 PWM value (tuning midpoint) for given frequency or 0 if tuning failed
 *
 */
uint8_t tune_rf(uint16_t freq) {
  uint16_t i = 0, low, high;

  pll_set_rf(freq, LMX2433_PRESCALER_8_9);

  set_resistor(BANDWADJ1_RES, 0);
  POWERCTL1_PORT |= _BV(POWERCTL1); // Turn on VCO1

#ifdef TEST
  OCR1A = 5;
#else
  OCR1A = 10;
#endif
  delay_ms(500);
  if (PLL_RFIN_PIN & _BV(PLL_RFIN)) { // Cannot tune any lower...???
    pc_puts_P(PSTR("RF VCO range is too high!\n\n"));
    return 0;
  }

#ifdef TEST
  OCR1A = 255;
#else
  OCR1A = 4095;
#endif
  delay_ms(500);
  if (!(PLL_RFIN_PIN & _BV(PLL_RFIN))) { // Cannot tune any higher...???
    pc_puts_P(PSTR("RF VCO range is too low!\n\n"));
    return 0;
  }

  pc_puts_P(PSTR("Midpoint at "));
  low = 0;
#ifdef TEST
  high = 255;
#else
  high = 4095;
#endif
  while ((low + 1) < high) {
    i = ((uint16_t)low + (uint16_t)high) / 2;
    OCR1A = i;
#ifdef DEBUG
    putnum_ud(low);
    uart_putchar('/');
    putnum_ud(high);
    uart_putchar('\t');
    putnum_ud(OCR1A);
    pc_puts(", ");
#endif
    delay_ms(500);
    if (PLL_RFIN_PIN & _BV(PLL_RFIN)) {
      delay_ms(1);
      if (PLL_RFIN_PIN & _BV(PLL_RFIN)) {
        high = i;
      }
    } else {
      low = i;
    }
  }
  putnum_ud(i);
  pc_putc('\n');

  return i;
}

/*
 * Tune PLL IF stage (VCO2) by finding a PWM value for a specific frequency.
 *
 * freq: Frequency to tune
 * Returns OCR1 PWM value (tuning midpoint) for given frequency or 0 if tuning failed
 *
 */
uint8_t tune_if(uint16_t freq) {
  uint16_t i = 0, low, high;

  pll_set_if(freq, LMX2433_PRESCALER_8_9);

  set_resistor(BANDWADJ2_RES, 0);
  POWERCTL2_PORT |= _BV(POWERCTL2); // Turn on VCO2

#ifdef TEST
  OCR1B = 5;
#else
  OCR1B = 10;
#endif
  delay_ms(500);
  if (PLL_IFIN_PIN & _BV(PLL_IFIN)) { // Cannot tune any lower...???
    pc_puts_P(PSTR("IF VCO range is too high!\n\n"));
    return 0;
  }

#ifdef TEST
  OCR1B = 255;
#else
  OCR1B = 4095;
#endif
  delay_ms(500);
  if (!(PLL_IFIN_PIN & _BV(PLL_IFIN))) { // Cannot tune any higher...???
    pc_puts_P(PSTR("IF VCO range is too low!\n\n"));
    return 0;
  }

  pc_puts_P(PSTR("Midpoint at "));
  low = 0;
#ifdef TEST
  high = 255;
#else
  high = 4095;
#endif
  while ((low + 1) < high) {
    i = ((uint16_t)low + (uint16_t)high) / 2;
    OCR1B = i;
#ifdef DEBUG
    putnum_ud(low);
    uart_putchar('/');
    putnum_ud(high);
    putnum_ud(OCR1B);
    pc_puts(", ");
#endif
    delay_ms(500);
    if (PLL_IFIN_PIN & _BV(PLL_IFIN)) {
      delay_ms(1);
      if (PLL_IFIN_PIN & _BV(PLL_IFIN)) {
        high = i;
      }
    } else {
      low = i;
    }
  }
  putnum_ud(i);
  pc_putc('\n');

  return i;
}

/*
 * Tune given VCO (RF stage) to a specific frequency bandwidth.
 *
 * min:     Minimum band frequency to tune
 * max:     Maximum band frequency to tune
 * vco_num: Number of VCO to tune, 0 or 1
 *
 * Returns average tuning value of digital potentiometer or 0 if tuning failed
 *
 */
uint8_t tune_rf_band(uint16_t min, uint16_t max, uint8_t vco_num) {
  uint16_t threshhold, midpt;
  uint32_t avg;
  uint8_t i = 0, j, low, high;

  // Check and correct possible frequency mismatch
  if (min > max) {
    midpt = max;
    max = min;
    min = midpt;
  }

  // Check for single frequency
  if (min == max) {
    midpt = min;
  } else {
    midpt = (min + max) / 2;
  }

  if (vco_num == 0) {
    midpt = tune_rf(midpt);
  } else {
    midpt = tune_if(midpt);
  }

  // Start in the middle
  if ((midpt == 0) || (min == max)) {
    return 0;
  }

  pc_puts_P(PSTR("\nBandwidth tuning...\n"));

  if (vco_num == 0) {
    pll_set_rf(min, 8);
  } else {
    pll_set_if(min, 8);
  }

  set_sawtooth_low();

  // Get high values?
  if (vco_num == 0) {
    set_resistor(BANDWADJ1_RES, 0);
    ADMUX = 0;
  } else {
    set_resistor(BANDWADJ2_RES, 0);
    ADMUX = 5;
  }
  delay_ms(100);
  avg = 0;
  for (j = 0; j < 127; ++j) {
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & _BV(ADSC)) {} // wait for conversion to finish
    avg += ADC;
#ifdef DEBUG
    putnum_ud(t);
    pc_putc(' ');
#endif
  }
  avg /= 128;
  threshhold = avg;
#ifdef DEBUG
  pc_puts("threshold = ");
  putnum_ud(threshhold);
  pc_putc('\n');
#endif

  low = 0;
  high = 255;
  while ((low + 1) < high) {
    i = ((uint16_t)low + (uint16_t)high) / 2;
    // Set the bandwidth
    if (vco_num == 0) {
      set_resistor(BANDWADJ1_RES, i);
    } else {
      set_resistor(BANDWADJ2_RES, i);
    }
#ifdef DEBUG
    putnum_ud(i);
    pc_puts(", ");
#endif
    delay_ms(500);

    // Read ADC
    if (vco_num == 0)
      ADMUX = 0;
    else
      ADMUX = 5;

    avg = 0;
    for (j = 0; j < 127; ++j) {
      ADCSRA |= _BV(ADSC);
      while (ADCSRA & _BV(ADSC)) {} // Wait for conversion to finish
      avg += ADC;
#ifdef DEBUG
      putnum_ud(t);
      uart_putchar(' ');
#endif
    }
    avg /= 128;
#ifdef DEBUG
    putnum_ud(avg);
    pc_putc('\n');
#endif
    if (avg < (threshhold - 10)) {
      high = i;
    } else {
      low = i;
    }
  }
  pc_puts_P(PSTR("Done! Variable resistor: "));
  putnum_ud(i);
  pc_puts_P(PSTR("\n\n"));
  set_sawtooth_high();

  return i;
}
