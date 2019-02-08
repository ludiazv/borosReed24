#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minipower.h"

// Only Pico Power devices can change BOD settings through software
/* #if defined __AVR_ATmega328P__ || defined (__AVR_ATmega328PB__) */
#ifndef sleep_bod_disable
#define sleep_bod_disable() 										\
do { 																\
  unsigned char tempreg; 													\
  __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" 			\
                       "ori %[tempreg], %[bods_bodse]" "\n\t" 		\
                       "out %[mcucr], %[tempreg]" "\n\t" 			\
                       "andi %[tempreg], %[not_bodse]" "\n\t" 		\
                       "out %[mcucr], %[tempreg]" 					\
                       : [tempreg] "=&d" (tempreg) 					\
                       : [mcucr] "I" _SFR_IO_ADDR(MCUCR), 			\
                         [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                         [not_bodse] "i" (~_BV(BODSE))); 			\
} while (0)
#endif


// Only Pico Power devices can change BOD settings through software
#if defined __AVR_ATmega328P__ || defined (__AVR_ATmega328PB__)
#define	lowPowerBodOff(mode)\
do { 						\
      set_sleep_mode(mode); \
      cli();				\
      sleep_enable();		\
			sleep_bod_disable(); \
      sei();				\
      sleep_cpu();			\
      sleep_disable();		\
      sei();				\
} while (0);
#endif

#define	lowPowerBodOn(mode)	\
do { 						\
      set_sleep_mode(mode); \
      cli();				\
      sleep_enable();		\
      sei();				\
      sleep_cpu();			\
      sleep_disable();		\
      sei();				\
} while (0);

// ------ IRQ --------
ISR (WDT_vect)
{
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	wdt_disable();
}

// ----- Functions ------
/*void idle(period_t p) {
  if (p != SLEEP_FOREVER)
	{
		wdt_enable(p);
		WDTCSR |= (1 << WDIE);	
	}
  lowPowerBodOn(SLEEP_MODE_IDLE);

}*/


void _sleep(period_t p,uint8_t mode) {
  if (p != SLEEP_FOREVER)
	{
		wdt_enable(p);
		WDTCSR |= (1 << WDIE);	
	}
    lowPowerBodOff(mode);
}
void powerDown(period_t period) {
	_sleep(period,SLEEP_MODE_PWR_DOWN);
}
void  adcNoiseReduction(period_t period) {
	_sleep(period,SLEEP_MODE_ADC);
}