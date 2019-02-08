#include "hal.hpp"


#include<avr/power.h>
#include<avr/sleep.h>
#include<avr/wdt.h>
#include <util/atomic.h>

#define BUT_DEBOUNCE   25    ///< Debounce button


void init_hw() {
    // Configure the IOs
    // -----------------
     // PORTB
     DDRB=  0b11101111;
     PORTB= 0b00000100;
     // PORTC
     DDRC=  0b0000110;
     //DDRC=  0b0000010;
     PORTC= 0b0000010;
     // PORTD
     DDRD=  0b11110010;
     PORTD= 0b00000000;
    // -----------------
#if defined(__AVR_ATmega328PB__)
    // PORTE 
    DDRE= 0b0000;
    PORTE= 0b0000;
 #endif

    // POWER Reduction
    // ----------------
    ADCSRA=0;
    power_adc_disable();
    power_usart0_disable();
    power_timer0_enable();  // Required by arduino millis
    power_timer1_disable();
    power_timer2_disable();

    #if defined(__AVR_ATmega328PB__)
        power_timer3_disable();
        power_timer4_disable();
        power_usart1_disable();
        power_ptc_disable();
        power_twi1_disable();
        power_spi1_disable();
        power_spi0_enable();
        #if defined(BOROSREED24_I2C)
            power_twi0_enable();
        #else
            power_twi0_disable();
        #endif
    #else
        power_spi_enable();     // Enable SPI as radio is SPI device
        #if defined(BOROSREED24_I2C)
            power_twi_enable();
        #else
            power_twi_disable();
        #endif
    #endif 

    // Configure ADC
    // -----------------
    // Disable AD7 & AD6 PIN for less power
    DIDR0 = 0b11000000;
    // MUX
    //ADMUX |= ( 1 << REFS1 ) | ( 1 << REFS0 );  // Internal REF 1.1 
    // PRESCALER
    #if F_CPU >= 8000000L
        ADCSRA |=  (1 << ADPS2) || (1 << ADPS1) || (1 << ADPS0);  // 128 prescaler
    #else
        ADCSRA |=  (1 << ADPS1) || (1 << ADPS0);        // 8 Prescaler at 1Mhz [@1Mhz -> 125Khz]
    #endif
    // -------------------

    // Interrupts
    // -------------------
    EIMSK = 0;                                // Mask external interrupts
    PCICR = 0;                                // Mask all PC Interrupts
    PCMSK0= 0;                                // Disable PCINT0
    PCMSK1= 0;                                // Disable PCINT1
    PCMSK2= 0;                                // Disable PCINT2
    EICRA = 0;                                // Ext interrupts at low level   
    EIFR  =  ( 1 << INTF1 ) | ( 1 << INTF0 ); // Clear interrupts
    //---------------------

    // WDT - disable
    wdt_disable();
}

void enable_uart() {
    power_usart0_enable();
}

// ADC Section
// -----------------
volatile static uint16_t _adc_val;          ///< Stores last ADC measure
volatile static uint8_t  _adc_samples;      ///< Sample counter

uint16_t getADC() { return _adc_val; }     
ISR(ADC_vect) {
    _adc_val += (ADCL | (ADCH << 8));
    if(_adc_samples>0) _adc_samples--;
}

static void _do_adc_sample(uint8_t samples) {
    _adc_val=0;
    _adc_samples=samples;
     do {
        //DBG_PRINT(_adc_samples);
        //DBG_PRINT(F("-ADCS:"));
        //DBG_PRINTLN(_adc_val);
        //DBG_FLUSH();
        //ADCSRA |= ( 1 << ADSC ); // Start conversion !! not needed as ADC starts when chip enter in noisereduction mode
        adcNoiseReduction(SLEEP_FOREVER); // Power save during conversion. timer is disabled 
    }while(_adc_samples>0);
}

void measureADC(uint8_t channel,uint8_t ref,uint8_t samples) {
    
    power_adc_enable(); // Enable power interrutps and conversion start
    ADMUX = ref | channel ; // Set channel and interal reference & channel
    ADCSRA |= ( 1 << ADEN ) |  ( 1 << ADIE); // Enable ADC Int
    delay(10); // Give time to stabilize reference
    _do_adc_sample((samples+1)>>1); // Discard samples/2
    _do_adc_sample(samples);    // Do Real measurement
    ADCSRA &= ~( (1 << ADEN ) | ( 1 << ADIE) );  // Disable interrupts and power off the ADC
    power_adc_disable(); // Disable power on the ADC
    /*DBG_PRINT(F("ADC:"));
    DBG_PRINT(_adc_val);
    DBG_PRINT("/"); DBG_PRINTLN(samples);
    DBG_FLUSH();*/
    _adc_val=_adc_val/samples; // Avg samples 
    
}
int16_t MeasureVBat(uint8_t samples) {
    measureADC(VBAT_ADC,ADC_INTERNAL_REF,samples);
    _adc_val= ((_adc_val * ADC_K) + 0.5f) ;
    return _adc_val;
}
int16_t MeasureVcc(uint8_t samples) {
    measureADC(VCC_ADC,ADC_VCC_REF,samples);
    return ADC_VCC_K/_adc_val;
}
// ----------------

// Actuators
// ------------
bool reed(bool inverted) {
    bool b= ( PINC & ( 1 << PINC3) );
    return (inverted) ? !b : b;
}
void wake(bool val) {
    if(val) PORTC |= (1 << PORTC1); else PORTC &= ~( 1 << PORTC1);
}
bool button(bool debounce) {
    bool b1= ( PINC & ( 1 << PINC0) );
    if(debounce) {
        delay(BUT_DEBOUNCE);
        bool b2= ( PINC & ( 1 << PINC0) );
        return (b1==b2) ? b1 : b2;
    } else return b1;
}
void led(uint8_t val){
    if(val) PORTD |= (1 << PORTD4); else PORTD &= ~( 1 << PORTD4);
}
// --------------

// Sleep
// -------
volatile static bool _enable_int1=false;
void enable_int1(bool val) {
    _enable_int1=val;
    if(val) PORTD |= (1 << PORTD3);  // Enable pull-up for INT1 
    else PORTD &= ~(1 << PORTD3);   
}
volatile static wake_t _wake;
ISR(INT1_vect) {
    cli();
    sleep_disable();
    EIMSK  &= ~( 1 << INT1);
    _wake=WAKE_EXTINT;
    sei();
}
ISR(PCINT1_vect) {
    cli();
    sleep_disable();
    if(PINC & ( 1 << PINC0)) _wake=WAKE_BUT;
    else _wake=WAKE_REED;
    sei();
}
ISR(INT0_vect) {
    cli();
    sleep_disable();
    EIMSK  &= ~( 1 << INT0);
    _wake = WAKE_RADIO;
    sei();
}
wake_t deepSleep(bool forever) {
    period_t per=(forever) ? SLEEP_FOREVER : SLEEP_8S;
    wake_t ret;
    _wake=WAKE_WDT;
    // Enable External interrupts
    if(_enable_int1) EIMSK  |= ( 1 << INT1); else EIMSK &= ~( 1 << INT1);
    PCICR  |= ( 1 << PCIE1);
    PCMSK1 |= (( 1 << PCINT11) | (1 << PCINT8)); //Enable pin-change int for but and reed
    powerDown(per);                             // ADC & BOD are disabled so let them as default
    ret=_wake;
    //DBG_PRINT("R:"); DBG_PRINT(ret);
    //DBG_FLUSH();
    // Disable wake interrupts
    EIMSK  &= ~( 1 << INT1);
    PCICR  &= ~( 1 << PCIE1);
    PCMSK1 &= ~( (1 << PCINT11) | (1 << PCINT8));
    return ret;
}

wake_t radioSleep() {
    wake_t ret;
    EIMSK |= ( 1<< INT0); // Enable interrupt of radio
    powerDown(SLEEP_60MS); // // ADC & BOD are disabled so let them as default
    ret=_wake;
    EIMSK  &= ~( 1 << INT0); // Disable radio interrupt
    return ret;
}

void reboot(){
    cli(); // Disable iterrupt as it a hard reset!
    wdt_enable(WDTO_120MS); 
    //wdt_reset();
    for(;;); // Wait the WDT
}

void patch_time(uint32_t ms){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { 
        extern volatile unsigned long timer0_millis; 
        extern volatile unsigned long timer0_overflow_count; 
        timer0_millis += ms; 
        // timer0 uses a /64 prescaler and overflows every 256 timer ticks 
        timer0_overflow_count += microsecondsToClockCycles(ms * 1000) / (64 * 256); 
    }
}