/**
 * \file pll.c
 *
 * PLL driver functions
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pll.h"
#include "main.h"
#include "serial.h"

/**
 * Init PLL
 *
 * Initialize PLL.
 *
*/
void pll_init(void) {
  uint32_t out;

  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2);

  PLLCLK_DDR |= _BV(PLLCLK);
  PLLDATA_DDR |= _BV(PLLDATA);
  PLLLE_DDR |= _BV(PLLLE);
  PLL_IFIN_DDR &= ~_BV(PLL_IFIN);
  PLL_RFIN_DDR &= ~_BV(PLL_RFIN);
  PLL_IFIN_PORT &= ~_BV(PLL_IFIN);
  PLL_RFIN_PORT &= ~_BV(PLL_RFIN);

  pll_tx(0x0, 0x2); // dont use general purpose pins, no fastlock
  pll_tx(0x0, 0x5); // dont use general purpose pins, no fastlock

  // setup 1MHz reference clock
  out = 3; out <<= 19; out |= (10&0x7FFF);
  pll_tx(out, 0x3); // no otherbits set: defaults
  out = 3; out <<= 19; out |= (10&0x7FFF);
  pll_tx(out, 0x0); // no otherbits set: defaults
}

/**
 * Send data word to PLL
 *
 * \param       data    Data word to send
 * \param       addr    PLL register to store data
 *
*/
void pll_tx(uint32_t data, uint8_t addr) {
  uint8_t i;

  if (addr > 5) return;

  data <<= 3;
  data |= (addr & 0x7);
  data <<= 8;

  PLLLE_PORT &= ~_BV(PLLLE);
  PLLDATA_PORT &= ~_BV(PLLDATA);
  PLLCLK_PORT &= ~_BV(PLLCLK);

  for (i=0; i<24; i++) {
    PLLCLK_PORT &= ~_BV(PLLCLK);
    if (data & 0x80000000) {
            PLLDATA_PORT |= _BV(PLLDATA);
    } else {
            PLLDATA_PORT &= ~_BV(PLLDATA);
    }
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    PLLCLK_PORT |= _BV(PLLCLK);
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    data <<= 1;
  }
  PLLLE_PORT |= _BV(PLLLE);
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  PLLLE_PORT &= ~_BV(PLLLE);
  PLLDATA_PORT &= ~_BV(PLLDATA);
  PLLCLK_PORT &= ~_BV(PLLCLK);
}


/**
 * Set reference counter
 *
 * Set PLL reference counter.
 *
*/
void pll_set_rcounter(uint16_t rcounter) {
  pll_tx((rcounter&0x7FFF), 0x0); // no otherbits set: defaults
  pll_tx((rcounter&0x7FFF), 0x3); // no otherbits set: defaults
}


void pll_set_freq(uint16_t rf_freq, uint8_t prescaler, uint8_t reg) {
  uint16_t N;
  uint16_t B, A;
  uint32_t out=0;

  if ((prescaler != 8) && (prescaler != 16))
    prescaler = 8;

  // Fin = N*fcomp
  N = rf_freq; // fcomp = 1MHz
  //  N = (P*B) + A
  if (prescaler == 8) {
    B = N / 8;
    A = N % 8;
  } else {
    B = N / 16;
    A = N % 16;
  }

  pc_puts_P(PSTR("PLL for RF freq ")); putnum_ud(N);
  pc_puts_P(PSTR("MHz & prescaler ")); putnum_ud(prescaler);
  pc_puts_P(PSTR(": B=")); putnum_ud(B);
  pc_puts_P(PSTR(" A=")); putnum_ud(A);
  pc_putc('\n');

  if (prescaler == 16)
    out = 1; // 16 prescale
  out <<= 15;
  out |= B; out<<= 4;
  out |= A&0xF;
  pll_tx(out, reg);
}

/**
 * Tune PLL RF stage
 *
 * Find PWM value for specific frequency.
 *
 * \param       freq    Frequency to tune
 * \return      OCR1 PWM value for given frequeny.
 *
 */
uint8_t tune_rf(uint16_t freq) {
  uint16_t i=0, low, high;

  pll_set_rf(freq, 8);

  set_resistor(BANDWADJ1_RES, 0);
  POWERCTL1_PORT |= _BV(POWERCTL1); // turn on vco

  OCR1A = 10;
  delay_ms(500);

  if (PLL_RFIN_PIN & _BV(PLL_RFIN)) {	// we cant tune any lower...???
    pc_puts_P(PSTR("RF VCO range is too high!\n"));
    return 0;
  }
  OCR1A = 4095;
  delay_ms(500);

  if (! (PLL_RFIN_PIN & _BV(PLL_RFIN))) {	// we cant tune any higher...???
    pc_puts_P(PSTR("RF VCO range is too low!\n"));
    return 0;
  }

  pc_puts_P(PSTR("midpoint @ "));
  low = 0;
  high = 4095;
  while ((low + 2) <= high) {
    //putnum_ud(low); uart_putchar('/'); putnum_ud(high); uart_putchar('\t');
    i = ((uint16_t)low+(uint16_t)high)/2;
    OCR1A = i;
    //putnum_ud(OCR1A); pc_puts(", ");
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
  putnum_ud(i); pc_putc('\n');
  return i;
}

/**
 * Tune PLL IF stage
 *
 * Find PWM value for specific frequency.
 *
 * \param       freq    Frequency to tune
 * \return      OCR1 PWM value for given frequeny.
 *
 */
uint8_t tune_if(uint16_t freq) {
  uint16_t i=0, low, high;

  pll_set_if(freq, 8);

  set_resistor(BANDWADJ2_RES, 0);
  POWERCTL2_PORT |= _BV(POWERCTL2); // turn on vco

  OCR1B = 10;
  delay_ms(500);

  if (PLL_IFIN_PIN & _BV(PLL_IFIN)) {	// we cant tune any lower...???
    pc_puts_P(PSTR("IF VCO range is too high!\n"));
    return 0;
  }

  OCR1B = 4095;
  delay_ms(500);

  if (! (PLL_IFIN_PIN & _BV(PLL_IFIN))) {	// we cant tune any higher...???
    pc_puts_P(PSTR("IF VCO range is too low!\n"));
    return 0;
  }

  pc_puts_P(PSTR("midpoint @ "));
  low = 0;
  high = 4095;
  while ((low + 2) <= high) {
    i = ((uint16_t)low+(uint16_t)high)/2;
    OCR1B = i;
    //putnum_ud(OCR1B); pc_puts(", ");
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
  putnum_ud(i); pc_putc('\n');
  return i;
}

/**
 * Tune specific RF band.
 *
 * \param       min     Minimum band frequency
 * \param       max     maximum band frequency
 * \param       vco_num Number of VCO to tune
 * \return      Average tuning value
 *
 */
uint8_t tune_rf_band(uint16_t min, uint16_t max, uint8_t vco_num) {
  uint16_t threshhold, midpt;
  uint32_t avg;
  uint8_t i=0, j, low, high;

  // Check and correct possible frequency mismatch
  if (min > max) {
    midpt = max;
    max = min;
    min = midpt;
  }

  // Check for single frequency
  if( min == max)
    midpt = min;
  else
    midpt = (min+max)/2;

  if (vco_num == 0)
    midpt = tune_rf(midpt);
  else
    midpt = tune_if(midpt);

  if ((midpt == 0) || (min == max)) // start in the middle
    return 0;

  pc_puts_P(PSTR("\nbandwidth tuning...\n"));

  if (vco_num==0)
    pll_set_rf(min, 8)
  else
    pll_set_if(min, 8);

  set_sawtooth_low();

  // get high vals?
  if (vco_num == 0) {
    set_resistor(BANDWADJ1_RES, 0);
    ADMUX = 0;
  } else {
    set_resistor(BANDWADJ2_RES, 0);
    ADMUX = 5;
  }
  delay_ms(100);
  avg = 0;
  for (j = 0; j < 127; j++) {
    ADCSRA |=  _BV(ADSC);
    while (ADCSRA & _BV(ADSC)); // wait for conversion to finish;
    avg += ADC;
    //putnum_ud(t); pc_putc(' ');
  }
  avg /= 128;
  threshhold = avg;
  //pc_puts("thres = "); putnum_ud(threshhold); pc_putc('\n');

  low = 0;
  high = 255;
  while ((low + 2) <= high) {
    i = ((uint16_t)low+(uint16_t)high)/2;
    // set the bandwidth
    if (vco_num == 0)
      set_resistor(BANDWADJ1_RES, i);
    else
      set_resistor(BANDWADJ2_RES, i);
    //putnum_ud(i); pc_puts(", ");
    delay_ms(500);

    // read ADC

    if (vco_num == 0)
      ADMUX = 0;
    else
      ADMUX = 5;

    avg = 0;
    for (j = 0; j < 127; j++) {
      ADCSRA |=  _BV(ADSC);
      while (ADCSRA & _BV(ADSC)); // wait for conversion to finish;
      avg += ADC;
      //putnum_ud(t); uart_putchar(' ');
    }
    avg /= 128;
    //putnum_ud(avg); pc_putc('\n');
    if (avg < (threshhold-10)) {
      high = i;
    } else {
      low = i;
    }
  }
  putnum_ud(i);
  pc_puts_P(PSTR(" done!\n"));
  set_sawtooth_high();
  return i;
}
