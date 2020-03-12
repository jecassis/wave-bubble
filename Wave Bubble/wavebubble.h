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

#define POWERON_DDR DDRB   // Power on direction
#define POWERON_PORT PORTB // Power on switch port
#define POWERON PB6        // Power on switch pin

#define PROGKEY_PIN PINB   // Program key input
#define PROGKEY_DDR DDRB   // Program key direction
#define PROGKEY_PORT PORTB // Program key port
#define PROGKEY PB4        // Program key pin

#define POWERCTL1_DDR DDRB   // Power control direction for VCO1
#define POWERCTL1_PORT PORTB // Power control port for VCO1
#define POWERCTL1 PB7        // Power control pin for VCO1

#define POWERCTL2_DDR DDRD   // Power control direction for VCO2
#define POWERCTL2_PORT PORTD // Power control port for VCO2
#define POWERCTL2 PD3        // Power control pin for VCO2

#define LEDDDR DDRD   // Direction for LED
#define LEDPORT PORTD // Port for LED
#define LED PD7       // Pin for LED

#define LOWBATT_ADC_MUX_SELECT 6
#define LOWBATT_MINIMUM 310

#ifdef TEST
extern volatile char in_char;
#else // OPERATION
// 14-byte structure holding jammer program settings in EEPROM
typedef struct {
  uint16_t startfreq1, endfreq1; // Start and end frequencies in MHz
  uint16_t dc_offset1;           // Values for the PWM
  uint8_t bandwidth1;            // Values for the potentiometer
  uint16_t startfreq2, endfreq2; // Start and end frequencies in MHz
  uint16_t dc_offset2;           // Values for the PWM
  uint8_t bandwidth2;            // Values for the potentiometer
} __attribute__((__packed__)) jammer_setting;

#define MAX_PROGRAMS 5 // ((E2END+1)/sizeof(jammer_setting))-1=((0x1FF+0x1)/0xE)-0x1=0x23=35

#define LOWBAND_VCO_LOW 345   // Low band VCO lowest frequency in MHz
#define LOWBAND_VCO_HIGH 1350 // Low band VCO highest frequency in MHz

#define HIGHBAND_VCO_LOW 1225  // High band VCO lowest frequency in MHz
#define HIGHBAND_VCO_HIGH 2715 // High band VCO highest frequency in MHz

extern void delay_ms(uint16_t ms);
#endif

#endif
