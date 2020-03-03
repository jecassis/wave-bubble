/**
 * \file main.c
 *
 * \mainpage Wave Bubble 2010 Firmware
 *
 * \version v1.0b \n
 *
 * Operational firmware to run the Wave Bubble 2010 hardware \n\n
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
#include <util/delay.h>
#include <avr/eeprom.h>
#include <ctype.h>
#include "serial.h"
#include "pll.h"
#include "main.h"

uint16_t EEMEM dummy = 0; //!< A dummy word is used at EEPROM address 0 to prevent corruption of data.
uint16_t EEMEM validity = 0; //!< Validity value to check for empty eeprom.
uint8_t EEMEM max_programs=0; //!< Number of programs in eeprom
uint8_t EEMEM curr_program=0; //!< Number of actual program in use

uint8_t EEMEM settings_ee; //!< Offset where we start to save out setting

volatile uint16_t global_delay = 0;  //!< milliseconds delay counter, decremented by ISR
volatile uint16_t led_delay = 0;  //!< LED blink timer
volatile uint16_t key_delay = 0; //!< Key press timer to debounce and detect short/long press
volatile uint8_t lowbatt_timer = 0; //!< 1s timer for low battery threshold

/**
 * Milliseconds delay function
 *
 * Uses 1ms system tick from timer0
 *
 * \param       ms      Number of milliseconds to delay
 *
 */
void delay_ms(uint16_t ms) {

  global_delay = ms;

  // global_delay will be decreased in timer0 ISR
  while (global_delay > 0)
    asm volatile ("nop");
}

/**
 * Power device off
 *
 */
static void power_off(void) {
  pc_puts_P(PSTR("Powering OFF...\n"));
  delay_ms(300);
  #ifdef HW_REV_A
    // Power OFF device
    POWERON_PORT |= _BV(POWERON);
  #else
    // Power OFF device
    POWERON_PORT &=~ _BV(POWERON);
  #endif
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
 * \param       rval    Resistor value to set
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
 * Check validity of EEPROM content and initialize if
 * necessary.
 *
 */
static void init_eeprom(void) {
  uint16_t temp;

  uint8_t *p = 0;

  // Read validity word
  temp = eeprom_read_word(&validity);

  // Check validity, BEEF is good here.
  if( temp != 0xEFBE ) {
    //Not correct? Init EEPROM.
    for(uint16_t i = 0; i < E2END+1; i++) eeprom_write_byte(p++, 0);
  }
  eeprom_write_word(&validity, 0xEFBE);
}

/**
 * Init variable DC generation via PWM from timer 1
 *
 */
static void init_pwm(void) {
  DDRB |= _BV(PB1);  // make OCR1A it an output
  DDRB |= _BV(PB2);  // make OCR1B it an output
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM12) | _BV(WGM11) | _BV(WGM10); // 10bit FastPWM
  TCCR1B = _BV(CS10);
  OCR1A = 0x0;  // change this value to adjust PWM.
  OCR1B = 0x0;  // change this value to adjust PWM.
}

/**
 * Print menu on terminal
 *
 */
static void print_menu(void) {
  print_div();
  pc_puts_P(PSTR(" p > Display progs\n"));
  pc_puts_P(PSTR(" a > Add prog\n"));
  pc_puts_P(PSTR(" d > Delete prog\n"));
  pc_puts_P(PSTR(" t > Tune prog\n"));
  pc_puts_P(PSTR(" e > Erase all\n"));
  pc_puts_P(PSTR(" q > Quit menu\n"));
  pc_puts_P(PSTR(" o > Power off\n"));
  print_div();
  pc_puts_P(PSTR("=> "));
}

/**
 * Print program details on terminal
 *
 * \param       setting Jammer setting struct
 * \param       n       Program number
 * \param       m       Total number of programs
 *
 */
static void print_program(jammer_setting *setting, uint8_t n, uint8_t m) {
  print_div();
  pc_puts_P(PSTR("Program #")); putnum_ud(n+1);
  pc_puts_P(PSTR(" of "));
  putnum_ud(m); pc_putc('\n');

  pc_puts_P(PSTR("High band VCO: "));
  if(setting->startfreq1 == 0) {
    pc_puts_P(PSTR("OFF"));
  } else {
    putnum_ud(setting->startfreq1);
    pc_puts_P(PSTR(" -> "));
    putnum_ud(setting->endfreq1);
    pc_puts_P(PSTR(" (")); putnum_ud(setting->dc_offset1);
    pc_puts_P(PSTR(", ")); putnum_ud(setting->bandwidth1);
    pc_putc(')');
  }
  pc_puts_P(PSTR("\nLow band VCO: "));
  if(setting->startfreq2 == 0) {
    pc_puts_P(PSTR("OFF"));
  } else {
    putnum_ud(setting->startfreq2);
    pc_puts_P(PSTR(" -> "));
    putnum_ud(setting->endfreq2);
    pc_puts_P(PSTR(" (")); putnum_ud(setting->dc_offset2);
    pc_puts_P(PSTR(", ")); putnum_ud(setting->bandwidth2);
    pc_putc(')');
  }
  pc_puts_P(PSTR("\n"));
}

/**
 * Print all programs on terminal
 *
 */
static void display_programs(void) {
  uint8_t i, progs;
  jammer_setting setting;

  progs = eeprom_read_byte(&max_programs);
  if(progs > MAX_PROGRAMS) progs = MAX_PROGRAMS;
  putnum_ud(progs);  pc_puts_P(PSTR(" of "));
  putnum_ud(MAX_PROGRAMS);  pc_puts_P(PSTR(" programs in memory\n\r"));
  for (i=0; i< progs; i++) {
          eeprom_read_block(&setting,
                  &settings_ee+sizeof(jammer_setting)*i,
                  sizeof(jammer_setting));

          print_program(&setting, i, progs);
  }
}

/**
 * Delete program from EEPROM
 *
 */
static void delete_program(void) {
  uint8_t n, max;
  jammer_setting setting;

  display_programs();
  print_div();
  pc_puts_P(PSTR("Delete #? "));
  n = pc_read16();
  max = eeprom_read_byte(&max_programs);
  if(max > MAX_PROGRAMS) max = MAX_PROGRAMS;

  if ( (n == 0) || (n > max)) {
    pc_puts_P(PSTR("\n\rInvalid\n\r"));
    return;
  }

  if (n == max) {
    // easy, just reduce program count
    eeprom_write_byte(&max_programs, max-1);
  } else {
      // Delete desired program
      for(; n < max; n++) {
        // Shift trailing program blocks
        eeprom_read_block(&setting,
                &settings_ee+sizeof(jammer_setting)*n,
                sizeof(jammer_setting));

        eeprom_write_block(&setting,
                  &settings_ee+(sizeof(jammer_setting)*(n-1)),
                  sizeof(jammer_setting));
        // Remove deleted program from count
        eeprom_write_byte(&max_programs, max-1);
      }
  }
}

/**
 * Tune setting on VCOs and save tuning values
 * \param       n       Number of program to tune
 * \param       save    If true, save back tuning values
 *
 */
static void tune_it (uint8_t n, uint8_t save) {
  jammer_setting setting;

  eeprom_read_block(&setting,
      &settings_ee+sizeof(jammer_setting)*n,
      sizeof(jammer_setting));
  // Tune high band VCO
  if ((setting.startfreq1 != 0) && (setting.endfreq1 != 0)) {
    setting.bandwidth1 = tune_rf_band(setting.startfreq1, setting.endfreq1, 0);
    setting.dc_offset1 = OCR1A;
  }
  // Tune low band VCO
  if ((setting.startfreq2 != 0) && (setting.endfreq2 != 0)) {
    setting.bandwidth2 = tune_rf_band(setting.startfreq2, setting.endfreq2, 1);
    setting.dc_offset2 = OCR1B;
  }
  if (save) {
    eeprom_write_block(&setting,
          &settings_ee+sizeof(jammer_setting)*n,
          sizeof(jammer_setting));
  }
}

/**
 * Tune specific program
 *
 */
static void tune_program(void) {
  uint8_t n;

  if (eeprom_read_byte(&max_programs) == 0) {
    pc_puts_P(PSTR("\nNo programs stored.\n"));
    return;
  }

  display_programs();
  print_div();
  pc_puts_P(PSTR("Tune prog # "));
  n = pc_read16();
  if (n > eeprom_read_byte(&max_programs)) {
    pc_puts_P(PSTR("\n\rInvalid #\n"));
  } else {
    tune_it(n-1, 1);
  }
}

/**
 * Add program to jammer and store in EEPROM.
 *
 */
static void add_program(void) {
  jammer_setting new_setting;
  uint8_t progs;

  progs = eeprom_read_byte(&max_programs);
  if(progs >= MAX_PROGRAMS) {
    pc_puts_P(PSTR("Memory full.\n"));
    return;
  }

  pc_puts_P(PSTR("Enter start and stop frequency for each VCO.\nEnter 0 to turn OFF VCO.\n"));
  pc_puts_P(PSTR("Low Band: 345-1350MHz - High Band 1225-2715MHz\n\n"));

highbandstart:
  pc_puts_P(PSTR("High band VCO\nstart: "));
  new_setting.startfreq1 = pc_read16();
  if(new_setting.startfreq1 == 0) {
    new_setting.endfreq1 = 0;
    goto lowbandstart;
  }
  if(new_setting.startfreq1 < HIGHBAND_VCO_LOW) {
    pc_puts_P(PSTR("Frequency to low.\n"));
    goto highbandstart;
  }
highbandend:
  pc_puts_P(PSTR("end: "));
  new_setting.endfreq1 = pc_read16();
  if(new_setting.endfreq1 > HIGHBAND_VCO_HIGH) {
    pc_puts_P(PSTR("Frequency to high.\n"));
    goto highbandend;
  }
lowbandstart:
  pc_puts_P(PSTR("\nLow band VCO\nstart: "));
  new_setting.startfreq2 = pc_read16();
  if(new_setting.startfreq2 == 0) {
    new_setting.endfreq2 = 0;
    goto saveprog;
  }
  if(new_setting.startfreq2 < LOWBAND_VCO_LOW) {
    pc_puts_P(PSTR("Frequency to low.\n"));
    goto lowbandstart;
  }
lowbandend:
  pc_puts_P(PSTR("end: "));
  new_setting.endfreq2 = pc_read16();
  if(new_setting.endfreq2 > LOWBAND_VCO_HIGH) {
    pc_puts_P(PSTR("Frequency to high.\n"));
    goto lowbandend;
  }
saveprog:
  if( (new_setting.startfreq1 == 0) && (new_setting.startfreq2 == 0)) {
    pc_puts_P(PSTR("Nothing to save.\n"));
    return;
  }
  new_setting.dc_offset1 = new_setting.dc_offset2 = 0;
  new_setting.bandwidth1 = new_setting.bandwidth2 = 0;

  eeprom_write_block(&new_setting,
            &settings_ee+(sizeof(jammer_setting)*progs),
            sizeof(jammer_setting));

  eeprom_write_byte(&max_programs, progs+1);
  pc_puts_P(PSTR("Saved program ")); putnum_ud(progs+1);

  pc_putc('\n');
}

/**
 * Print menu and wait for users input.
 *
 */
static void run_menu(void) {
  char c;
  do {
    print_menu();
    c = pc_getc();
    pc_putc(c);
    pc_putc('\n');
    switch (c) {
      case 'p':
        display_programs();
        break;
      case 'a':
        add_program();
        break;
      case 'd':
        delete_program();
        break;
      case 't':
        tune_program();
        break;
      case 'e':
        pc_puts_P(PSTR("Erase all programs? y/n: "));
        c = pc_getc();
        pc_putc(c);
        pc_putc('\n');
        if(c == 'y') {
          eeprom_write_word(&validity, 0x0000);
          init_eeprom();
        }
        break;
      case 'q': break;
      case 'o':
        power_off();
        break;
      default:
        pc_puts_P(PSTR("Bad command\n"));
    }
  } while (c != 'q');
}


/**
 * The main function.
 *
 * \return Zero
 *
 */
int main(void) {

  OSCCAL = 0xC0; // Calibrate internal RC oscillator

  // setup system tick counter
  OCR0A = 125; // timer0 capture at 1ms
  TCCR0A |= _BV(WGM01);
  TCCR0B |=  _BV(CS01) | _BV(CS00); // Set timer0 CTC mode, prescaler 64
  TIMSK0 |= _BV (OCIE0A); // Enable timer0 output compare interrupt

  sei(); // Enable global interrupts

  key_delay = 0;
  // Power key must be pressed at least 2 seconds to power ON
  while(key_delay < 2000) asm volatile ("nop");

  // Power ON switch pin output and force HIGH to keep device running
  POWERON_PORT |= _BV(POWERON);
  POWERON_DDR |= _BV(POWERON);

  usart_init(); // Init serial communication

  DDRD |= _BV(PD1);     // USART TxD0 output

  // LED pin output and LED on
  LEDDDR |= _BV(LED);
  LEDPORT |= _BV(LED);

  // Program key input and pull-up
  PROGKEY_PORT |= _BV(PROGKEY);
  PROGKEY_DDR &=~ _BV(PROGKEY);

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

  init_eeprom();  // Check EEPROM validity

  pc_puts_P(PSTR("Wave Bubble 2010\nFW: " __DATE__" / " __TIME__"\n\n"));

  uint8_t progs, programnum;
  jammer_setting setting;

  progs = eeprom_read_byte(&max_programs);
  if(progs > MAX_PROGRAMS) progs = MAX_PROGRAMS;

  if  (progs != 0) {
    pc_puts_P(PSTR("Press key to enter menu..."));
    delay_ms(2000);
  }

no_progs:       // Go here in case 'q' is pressed and no programs are stored

  if ( (UCSR0A & _BV(RXC0)) && isascii(pc_getc()) ) {
    pc_putc('\n');
    run_menu();
  } else if (progs == 0)
    run_menu();

run_prog:       // Go here when program key is pressed, switch to next program

  progs = eeprom_read_byte(&max_programs);
  if(progs == 0) {
    pc_puts_P(PSTR("No programs stored.\n"));
    goto no_progs;
  }
  if(progs > MAX_PROGRAMS) progs = MAX_PROGRAMS;

  programnum = eeprom_read_byte(&curr_program);
  if (programnum >= progs)
    programnum = 0;

  eeprom_read_block(&setting,
            &settings_ee+sizeof(jammer_setting)*programnum,
            sizeof(jammer_setting));

  pc_putc('\n');
  print_program(&setting, programnum, progs);

  print_div();

  if ((setting.dc_offset1 == 0) && (setting.bandwidth1 == 0) &&
    (setting.dc_offset2 == 0) && (setting.bandwidth2 == 0)) {
            tune_it(programnum, 0); // mem check return
  } else {
    if (setting.startfreq1 != 0) {
            OCR1A = setting.dc_offset1;
            set_resistor(BANDWADJ1_RES, setting.bandwidth1);
            POWERCTL1_PORT |= _BV(POWERCTL1); // turn on vcos/gain
    }
    if (setting.startfreq2 != 0) {
            OCR1B = setting.dc_offset2;
            set_resistor(BANDWADJ2_RES, setting.bandwidth2);
            POWERCTL2_PORT |= _BV(POWERCTL2); // turn on vcos/gain
    }
  }

  uint8_t key_press = 0, led_count = 0;

  for(;;) {
    // check battery voltage
    uint16_t batt = 0;
    ADMUX = 6;
    ADCSRA |=  _BV(ADSC);
    while (ADCSRA & _BV(ADSC)); // wait for conversion to finish
    batt = ADC;
    //putnum_ud(batt); pc_putc('\n');

    // Reset low battery timer as long as battery voltage is good
    if(batt > 310) lowbatt_timer = 100; // 100 x 10ms time for low batt before power off

    if(lowbatt_timer == 0)
    {
      #ifdef HW_REV_A
        // Power OFF device
        POWERON_PORT |= _BV(POWERON);
      #else
        // Power OFF device
        POWERON_PORT &=~ _BV(POWERON);
      #endif
      while(1);
    }

    // check if a key was pressed earlier
    if (key_press) {
      // check if key has been realesed
      if (bit_is_set (PROGKEY_PIN, PROGKEY)) {
        // key is released
        if ((key_delay > 0) && (key_delay < 800)) {       // max 0.8 sec for shortpress
          LEDPORT &=~ _BV(LED); // turn LED off in any case
          POWERCTL1_PORT &=~ _BV(POWERCTL1); // turn VCOs off
          POWERCTL2_PORT &=~ _BV(POWERCTL2); // will be turned on dependent on next program
          eeprom_write_byte(&curr_program, programnum+1);
          goto run_prog;
        }
        key_press = 0;
      }
      else if ((key_delay > 800) && (key_delay < 5000)) { // long keypress ?
        power_off();
      }
    }
    else {
      // check if key is pressed
      if (bit_is_clear (PROGKEY_PIN, PROGKEY)) {
          key_delay = 0; // reset key press timer
          key_press = 1;
      }
    }

    /* Indicate selected program with blinking LED
     *
     * LED will blink program number of times followed by 1s pause
     */
    if( led_delay == 0) {
      if( led_count == 0) {
        led_count = programnum+1;
        led_delay = 1000;
      }
      else {
        if(bit_is_clear(LEDPORT,LED)) {
          LEDPORT |= _BV(LED);
          if(led_count == 0) {
            led_delay = 1000;
          }
          else {
            led_delay = 100;
          }
        }
        else {
          LEDPORT &=~ _BV(LED);
          led_delay = 300;
          --led_count;
        }
      }
    }

  } // end for(;;)

  return 0;
}


/**
 * Timer0 OCR interrupt service routine
 *
 * System tick 1ms
 *
 */
ISR(TIMER0_COMPA_vect)
{
  if (global_delay > 0) --global_delay;
  if (led_delay > 0) --led_delay;
  if (lowbatt_timer > 0) --lowbatt_timer;
  key_delay++;
}
