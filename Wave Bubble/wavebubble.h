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

#define SPICLK_DDR DDRC   // SPI CLOCK direction for digitally-controlled variable resistor
#define SPICLK_PORT PORTC // SPI CLOCK port for digitally-controlled variable resistor
#define SPICLK PC1        // SPI CLOCK pin for digital digitally-controlled variable resistor

#define SPISDO_DDR DDRC   // SPI SDO direction for digitally-controlled variable resistor
#define SPISDO_PORT PORTC // SPI SDO port for digitally-controlled variable resistor
#define SPISDO PC2        // SPI SDO pin for digitally-controlled variable resistor

#define SPICS_DDR DDRC   // SPI CS direction for digitally-controlled variable resistor
#define SPICS_PORT PORTC // SPI CS port for digitally-controlled variable resistor
#define SPICS PC3        // SPI CS pin for digitally-controlled variable resistor

#define FREQSET_DDR DDRC   // Digital frequency set direction
#define FREQSET_PORT PORTC // Digital frequency set port
#define FREQSET PC4        // Digital frequency set pin

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

#define AD8402_REG_MASK 0x03FF
#define AD8402_SHIFT_REGISTER_SIZE 10
#define AD8402_MSB_MASK (1 << (AD8402_SHIFT_REGISTER_SIZE - 1))
#define AD8402_ADDRESS_BIT_SHIFT 8
#define AD8402_NUMBER_ADDRESS_BITS 2
#define AD8402_ADDRESS_SIZE_MASK 0x3

#define AD8402_RDAC1_ADDRESS 0x0
#define AD8402_RDAC2_ADDRESS 0x1

#define BANDWADJ1_RES AD8402_RDAC2_ADDRESS // Digital potentiometer for VCO1
#define BANDWADJ2_RES AD8402_RDAC1_ADDRESS // Digital potentiometer for VCO2

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

extern void init_pwm(void);
extern void set_sawtooth_low(void);
extern void set_sawtooth_high(void);
extern void set_resistor(uint8_t rnum, uint8_t rval);

/* clang-format off */
#define nop() asm volatile ("nop")
/* clang-format on */

#endif
