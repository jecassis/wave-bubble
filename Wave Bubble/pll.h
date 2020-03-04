/*
 * pll.h
 *
 * PLL header file.
 *
 */

#ifndef PLL_H
#define PLL_H

#include <avr/io.h>

#define PLLCLK_DDR DDRD   // PLL clock direction port
#define PLLCLK_PORT PORTD // PLL clock output port
#define PLLCLK PD5        // PLL clock pin

#define PLLDATA_DDR DDRD   // PLL data direction port
#define PLLDATA_PORT PORTD // PLL data output port
#define PLLDATA PD6        // PLL data pin

#define PLLLE_DDR DDRB   // PLL enable direction port
#define PLLLE_PORT PORTB // PLL enable output port
#define PLLLE PB0        // PLL enable pin

#define PLL_RFIN_DDR DDRC   // PLL RF lock detect direction port
#define PLL_IFIN_DDR DDRC   // PLL IF lock detect direction port
#define PLL_RFIN PC0        // PLL RF lock detect pin
#define PLL_IFIN PC5        // PLL IF lock detect pin
#define PLL_RFIN_PIN PINC   // PLL RF input port
#define PLL_IFIN_PIN PINC   // PLL IF input port
#define PLL_RFIN_PORT PORTC // PLL RF output port
#define PLL_IFIN_PORT PORTC // PLL IF output port

extern void pll_tx(uint32_t data, uint8_t addr);
extern void pll_init(void);
extern void pll_set_rcounter(uint16_t rcounter);
extern void pll_set_freq(uint16_t rf_freq, uint8_t prescaler, uint8_t reg);
extern uint8_t tune_rf(uint16_t freq);
extern uint8_t tune_if(uint16_t freq);
extern uint8_t tune_rf_band(uint16_t min, uint16_t max, uint8_t vco_num);

#define pll_set_rf(x, n) pll_set_freq(x, n, 0x1);
#define pll_set_if(x, n) pll_set_freq(x, n, 0x4);

#endif
