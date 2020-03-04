/*
 * wavebubble.h
 *
 * Wave Bubble header file.
 *
 */

#ifndef WAVEBUBBLE_H
#define WAVEBUBBLE_H

#include <avr/io.h>

#define HW_REV_B // Wave Bubble 2010 hardware revision

#define SPICLK_PORT PORTC // SPI CLOCK Port for digital pot
#define SPIDO_PORT PORTC  // SPI DATA Port for digital pot
#define SPICS_PORT PORTC  // SPI CHIPSELECT Port for digital pot
#define SPICLK_DDR DDRC   // SPI CLOCK Direction for digital pot
#define SPIDO_DDR DDRC    // SPI DATA Direction for digital pot
#define SPICS_DDR DDRC    // SPI CHIPSELECT Direction for digital pot
#define SPICLK 1          // SPI CLOCK Pin for digital pot
#define SPIDO 2           // SPI DATA Pin for digital pot
#define SPICS 3           // SPI CHIPSELECT Pin for digital pot

#define BANDWADJ1_RES 1 // Digital potentiometer for VCO1
#define BANDWADJ2_RES 0 // Digital potentiometer for VCO2

#define FREQSET_DDR DDRC   // Digital frequency set direction
#define FREQSET_PORT PORTC // Digital frequency set port
#define FREQSET PC4        // Digital frequency set pin

#define POWERON_PORT PORTB // Power ON switch port
#define POWERON PB6        // Power ON switch pin
#define POWERON_DDR DDRB   // Power ON direction

#define PROGKEY_PORT PORTB // Program key port
#define PROGKEY_PIN PINB   // Program key input
#define PROGKEY PB4        // Program key pin
#define PROGKEY_DDR DDRB   // Program key direction

#define POWERCTL1_PORT PORTB // Power control port VCO1
#define POWERCTL1 PB7        // Power control pin VCO1
#define POWERCTL1_DDR DDRB   // Power control direction VCO1

#define POWERCTL2_PORT PORTD // Power control port VCO2
#define POWERCTL2 PD3        // Power control pin VCO2
#define POWERCTL2_DDR DDRD   // Power control direction VCO2

#define LEDDDR DDRD   // Direction for LED
#define LEDPORT PORTD // Port for LED
#define LED PD7       // Pin for LED

#ifdef TEST
extern volatile char in_char;
#else // OPERATION
// 12-byte structure holding jammer program settings in EEPROM
typedef struct {
  uint16_t startfreq1, endfreq1; // Start and end frequencies in MHz
  uint16_t dc_offset1;           // Values for the PWM
  uint8_t bandwidth1;            // Values for the potentiometer
  uint16_t startfreq2, endfreq2; // Start and end frequencies in MHz
  uint16_t dc_offset2;           // Values for the PWM
  uint8_t bandwidth2;            // Values for the potentiometer
} __attribute__((__packed__)) jammer_setting;

#define MAX_PROGRAMS 5 // ((E2END+1)/sizeof(jammer_setting))-1

#define LOWBAND_VCO_LOW 345   // Low band VCO lowest frequency in MHz
#define LOWBAND_VCO_HIGH 1350 // Low band VCO highest frequency in MHz

#define HIGHBAND_VCO_LOW 1225  // High band VCO lowest frequency in MHz
#define HIGHBAND_VCO_HIGH 2715 // High band VCO highest frequency in MHz

extern void delay_ms(uint16_t ms);
#endif

extern void init_pwm(void);
extern void set_sawtooth_low(void);
extern void set_sawtooth_high(void);
extern void set_resistor(uint8_t rnum, uint8_t rval);

#endif
