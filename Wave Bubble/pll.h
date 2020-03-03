/**
 * \file pll.h
 *
 * PLL header file
 *
 */
#define pll_set_rf(x, n) pll_set_freq(x, n, 0x1);
#define pll_set_if(x, n) pll_set_freq(x, n, 0x4);

#define PLLCLK_DDR DDRD
#define PLLCLK_PORT PORTD
#define PLLCLK PD5

#define PLLDATA_DDR DDRD
#define PLLDATA_PORT PORTD
#define PLLDATA PD6

#define PLLLE_DDR DDRB
#define PLLLE_PORT PORTB
#define PLLLE PB0

#define PLL_RFIN_DDR DDRC
#define PLL_IFIN_DDR DDRC
#define PLL_RFIN PC0
#define PLL_IFIN PC5
#define PLL_RFIN_PIN PINC
#define PLL_IFIN_PIN PINC
#define PLL_RFIN_PORT PORTC
#define PLL_IFIN_PORT PORTC

extern void pll_tx(uint32_t data, uint8_t addr) ;
extern void pll_init(void) ;
extern void pll_set_rcounter(uint16_t rcounter);
extern void pll_set_freq(uint16_t rf_freq, uint8_t prescaler, uint8_t reg);
extern uint8_t tune_rf(uint16_t freq);
extern uint8_t tune_if(uint16_t freq);
extern void pll_init(void);
extern uint8_t tune_rf_band(uint16_t min, uint16_t max, uint8_t vco_num);
