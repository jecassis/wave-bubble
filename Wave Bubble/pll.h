/*
 * pll.h
 *
 * PLL header file.
 *
 */

#ifndef PLL_H
#define PLL_H

#include <avr/io.h>

#define PLLCLK_DDR DDRD   // PLL CLK direction
#define PLLCLK_PORT PORTD // PLL CLK port
#define PLLCLK PD5        // PLL CLK pin

#define PLLDATA_DDR DDRD   // PLL DATA direction
#define PLLDATA_PORT PORTD // PLL DATA port
#define PLLDATA PD6        // PLL DATA pin

#define PLLLE_DDR DDRB   // PLL LE direction
#define PLLLE_PORT PORTB // PLL LE port
#define PLLLE PB0        // PLL LE pin

#define PLL_RFIN_PIN PINC   // PLL RF lock detect input
#define PLL_RFIN_DDR DDRC   // PLL RF lock detect direction
#define PLL_RFIN_PORT PORTC // PLL RF lock detect port
#define PLL_RFIN PC0        // PLL RF lock detect pin

#define PLL_IFIN_PIN PINC   // PLL IF lock detect input
#define PLL_IFIN_DDR DDRC   // PLL IF lock detect direction
#define PLL_IFIN_PORT PORTC // PLL IF lock detect port
#define PLL_IFIN PC5        // PLL IF lock detect pin

#define LMX2433_REG_MASK 0xFFFFFF
#define LMX2433_PRESCALER_8_9 8
#define LMX2433_PRESCALER_16_17 16
#define LMX2433_SHIFT_REGISTER_SIZE 24
#define LMX2433_MSB_MASK (1UL << (LMX2433_SHIFT_REGISTER_SIZE - 1))
#define LMX2433_NUMBER_ADDRESS_BITS 3
#define LMX2433_ADDRESS_MASK 0x7

#define LMX2433_R0_RF_R_ADDRESS 0x0
#define LMX2433_R1_RF_N_ADDRESS 0x1
#define LMX2433_R2_RF_TOC_ADDRESS 0x2
#define LMX2433_R3_IF_R_ADDRESS 0x3
#define LMX2433_R4_IF_N_ADDRESS 0x4
#define LMX2433_R5_IF_TOC_ADDRESS 0x5

/* Control Register R0/R3 bits */
#define LMX2433_R0_R3_R_BIT_SHIFT 3
#define LMX2433_R0_R3_R_MASK 0x7FFF

#define LMX2433_R0_R3_CPP_PHASE_DETECT_POSITIVE_POLARITY 1UL
#define LMX2433_R0_R3_CPP_PHASE_DETECT_NEGATIVE_POLARITY 0UL
#define LMX2433_R0_R3_CPP_PHASE_DETECT_BIT_SHIFT 18
#define LMX2433_R0_R3_CPP_PHASE_DETECT_MASK 0x1

#define LMX2433_R0_R3_CPG_CHARGE_PUMP_GAIN_4MA 1UL
#define LMX2433_R0_R3_CPG_CHARGE_PUMP_GAIN_1MA 0UL
#define LMX2433_R0_R3_CPG_CHARGE_PUMP_GAIN_BIT_SHIFT 19
#define LMX2433_R0_R3_CPG_CHARGE_PUMP_GAIN_MASK 0x1

#define LMX2433_R0_R3_CPT_CHARGE_PUMP_TRISTATE_HI 1UL
#define LMX2433_R0_R3_CPT_CHARGE_PUMP_TRISTATE_NORMAL 0UL
#define LMX2433_R0_R3_CPT_CHARGE_PUMP_TRISTATE_BIT_SHIFT 20
#define LMX2433_R0_R3_CPT_CHARGE_PUMP_TRISTATE_MASK 0x1

#define LMX2433_R0_R3_RST_SYNTH_COUNTER_RESET_ENABLE 1UL
#define LMX2433_R0_R3_RST_SYNTH_COUNTER_RESET_NORMAL 0UL
#define LMX2433_R0_R3_RST_SYNTH_COUNTER_RESET_BIT_SHIFT 21
#define LMX2433_R0_R3_RST_SYNTH_COUNTER_RESET_MASK 0x1

#define LMX2433_MUX_RF_PLL_DIGITAL_LOCK_DETECT 4UL
#define LMX2433_MUX_IF_PLL_DIGITAL_LOCK_DETECT 5UL
#define LMX2433_MUX_RF_PLL_ANALOG_LOCK_DETECT 7UL
#define LMX2433_MUX_IF_PLL_ANALOG_LOCK_DETECT 8UL
#define LMX2433_MUX_RF_N_DIV_BY_2 15UL
#define LMX2433_R0_R3_MUX_BIT_SHIFT 22
#define LMX2433_R0_R3_MUX_MASK 0x3
#define LMX2433_R0_MUX_BITS(select) (uint32_t)(select & LMX2433_R0_R3_MUX_MASK)
#define LMX2433_R3_MUX_BITS(select) (uint32_t)((select >> 2) & LMX2433_R0_R3_MUX_MASK)

/* Control Register R1/R4 bits */
#define LMX2433_R1_R4_A_BIT_SHIFT 3
#define LMX2433_R1_R4_A_MASK 0xF

#define LMX2433_R1_R4_B_BIT_SHIFT 7
#define LMX2433_R1_B_MASK 0x7FFF
#define LMX2433_R4_B_MASK 0x3FFF

#define LMX2433_R1_R4_SYNTH_PRESCALE_SELECT_8_9 0UL
#define LMX2433_R1_R4_SYNTH_PRESCALE_SELECT_16_17 1UL
#define LMX2433_R1_R4_SYNTH_PRESCALE_BIT_SHIFT 22
#define LMX2433_R1_R4_SYNTH_PRESCALE_MASK 0x1

#define LMX2433_R1_R4_SYNTH_PD_PLL_ACTIVE 0UL
#define LMX2433_R1_R4_SYNTH_PD_PLL_POWERDOWN 1UL
#define LMX2433_R1_R4_SYNTH_PD_BIT_SHIFT 23
#define LMX2433_R1_R4_SYNTH_PD_MASK 0x1

/* Control Register R2/R5 bits */
#define LMX2433_R2_R5_TOC_BIT_SHIFT 3
#define LMX2433_R2_R5_TOC_MASK 0xFFF

extern void pll_tx(uint32_t data, uint8_t addr);
extern void pll_init(void);
extern void pll_set_freq(uint16_t rf_freq, uint8_t prescaler, uint8_t reg);
extern uint8_t tune_rf(uint16_t freq);
extern uint8_t tune_if(uint16_t freq);
extern uint8_t tune_rf_band(uint16_t min, uint16_t max, uint8_t vco_num);

#define pll_set_rf(x, n) pll_set_freq(x, n, LMX2433_R1_RF_N_ADDRESS)
#define pll_set_if(x, n) pll_set_freq(x, n, LMX2433_R4_IF_N_ADDRESS)

#endif
