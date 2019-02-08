/**
 * @file minipower.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-01-31
 * 
 * @copyright Copyright (c) 2019
 * 
 *  A mini low-power lib based in rocketstream Low-Power
 * 
 */
#ifndef _H_MINIPOWER_H_
#define _H_MINIPOWER_H_


typedef enum period_te
{
	SLEEP_15MS,
	SLEEP_30MS,	
	SLEEP_60MS,
	SLEEP_120MS,
	SLEEP_250MS,
	SLEEP_500MS,
	SLEEP_1S,
	SLEEP_2S,
	SLEEP_4S,
	SLEEP_8S,
	SLEEP_FOREVER
} period_t;

// Prototypes
void idle(period_t period);
void powerDown(period_t period);
void adcNoiseReduction(period_t period);

// Tweaks for ATMEGA328PB
#if defined(__AVR_HAVE_PRR0_PRSPI0)
#define power_spi0_enable()      (PRR0 &= (uint8_t)~(1 << PRSPI0))
#define power_spi0_disable()     (PRR0 |= (uint8_t)(1 << PRSPI0))
#endif

#if defined(__AVR_HAVE_PRR1_PRSPI1)
#define power_spi1_enable()      (PRR1 &= (uint8_t)~(1 << PRSPI1))
#define power_spi1_disable()     (PRR1 |= (uint8_t)(1 << PRSPI1))
#endif

#if defined(__AVR_HAVE_PRR0_PRTWI0)
#define power_twi0_enable()      (PRR0 &= (uint8_t)~(1 << PRTWI0))
#define power_twi0_disable()     (PRR0 |= (uint8_t)(1 << PRTWI0))
#endif

#if defined(__AVR_HAVE_PRR1_PRTWI1)
#define power_twi1_enable()      (PRR1 &= (uint8_t)~(1 << PRTWI1))
#define power_twi1_disable()     (PRR1 |= (uint8_t)(1 << PRTWI1))
#endif

#if defined(__AVR_HAVE_PRR1_PRPTC)
#define power_ptc_enable()      (PRR1 &= (uint8_t)~(1 << PRPTC))
#define power_ptc_disable()     (PRR1 |= (uint8_t)(1 << PRPTC))
#endif


#endif