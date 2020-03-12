/*
 * test.h
 *
 * Hardware test header file.
 *
 */

#ifndef TEST_H
#define TEST_H

#include <avr/io.h>

extern void delay_ms(uint16_t ms);
extern void test_led(void);
extern void test_resistors(void);
extern void test_powerswitch1(void);
extern void test_powerswitch2(void);
extern void test_dc(void);
extern void test_vcos(void);
extern void test_pll1(void);
extern void test_pll2_rf(void);
extern void test_pll2_if(void);

#endif
