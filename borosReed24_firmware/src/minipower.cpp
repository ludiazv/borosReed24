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
volatile static bool _wdt_no_ovf;
ISR (WDT_vect)
{
	// WDIE & WDIF is cleared in hardware upon entering this ISR
	wdt_disable();
  _wdt_no_ovf=false;
}

// ----- Functions ------
void idle(period_t p) {
  wdt_disable();
  if (p != SLEEP_FOREVER)
	{
		_wdt_no_ovf=true;
    wdt_enable(p);
		WDTCSR |= (1 << WDIE);
    do { 
      lowPowerBodOn(SLEEP_MODE_IDLE);	
    } while(_wdt_no_ovf);
  }
}

#if defined(USE_32KHz)
void initTimer2(period_t p) {
  power_timer2_enable();
  TCCR2B =p ; // Normal mode . arduino set it  to mode 1
  TCCR2A =0 ; // Normal mode . arduino set it  to mode 1
  ASSR  = (1<<AS2);  //Enable asynchronous mode
  TCNT2=0; //set initial counter value
  while (ASSR & ((1<<TCN2UB)|(1<<TCR2BUB))); // wait for registers update
  TIFR2    =  (1<<TOV2); //clear interrupt flags
  TIMSK2  |=  (1<<TOIE2); //enable TOV2 interrupt
}
void stopTimer2(period_t p) {
  TCCR2B   = 0; // Stop clocking the timer
  TIFR2    = (1 << TOV2); //clear interrupt flags
  TIMSK2  &= ~(1 << TOIE2); //disable TOV2 interrupt
  power_timer2_disable();
}
#endif

void _sleep(period_t p,uint8_t mode) {

#if defined(USE_32KHZ)
  if(p != SLEEP_FOREVER) startTimer2(p);
#else
  wdt_disable(); // Make sure is disabled the W
  if (p != SLEEP_FOREVER)
	{
		wdt_enable(p);
		WDTCSR |= (1 << WDIE);	
	}
#endif

  lowPowerBodOff(mode);

#if defined(USE_32KHZ)
  if(p != SLEEP_FOREVER) stopTimer2();
#endif


}

void powerDown(period_t period) {
	_sleep(period,SLEEP_MODE_PWR_DOWN);
}

void  adcNoiseReduction(period_t period) {
	_sleep(period,SLEEP_MODE_ADC);
}