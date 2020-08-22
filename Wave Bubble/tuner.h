/*
 * tuner.h
 *
 * Tuning subsystem component functions header file.
 *
 */

#ifndef TUNER_H
#define TUNER_H

#include <avr/io.h>

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

#define AD8402_REG_MASK 0x3FF
#define AD8402_SHIFT_REGISTER_SIZE 10
#define AD8402_MSB_MASK (1 << (AD8402_SHIFT_REGISTER_SIZE - 1))
#define AD8402_ADDRESS_BIT_SHIFT 8
#define AD8402_NUMBER_ADDRESS_BITS 2
#define AD8402_ADDRESS_SIZE_MASK 0x3

#define AD8402_RDAC1_ADDRESS 0x0
#define AD8402_RDAC2_ADDRESS 0x1

#define BANDWADJ1_RES AD8402_RDAC2_ADDRESS // Digital potentiometer for VCO1
#define BANDWADJ2_RES AD8402_RDAC1_ADDRESS // Digital potentiometer for VCO2

extern void init_pwm(void);
extern void set_sawtooth_low(void);
extern void set_sawtooth_high(void);
extern void set_resistor(uint8_t rnum, uint8_t rval);

#endif
