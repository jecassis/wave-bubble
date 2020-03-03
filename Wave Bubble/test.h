/**
 * \file test.h
 *
 * Hardware test header file
 *
 */
#ifndef TEST_H_
#define TEST_H_

extern void test_led(void);
extern void set_sawtooth_low(void);
extern void set_sawtooth_high(void);
extern void set_resistor(uint8_t rnum, uint8_t rval);
extern void test_resistors(void);
extern void test_powerswitch1(void);
extern void test_powerswitch2(void);
extern void test_dc(void);
extern void test_vcos(void);
extern void delay_ms(uint16_t ms);
extern void test_pll1(void);
extern void test_pll2_rf(void);
extern void test_pll2_if(void);

#endif /* TEST_H_ */
