#ifndef _H_BOROSREED24_HAL_H_
#define _H_BOROSREED24_HAL_H_

#include <Arduino.h>
#include "minipower.h"
#include "radio.hpp"
#include "dbg.h"

#define VBAT_ADC        6
#define VCC_ADC         0xE
//#define VBAT_ADC        2
#define TEMP_ADC        8
 
#define VBAT_ALARM      2000

#define ADC_INTERNAL_REF ( ( 1 << REFS1 ) | ( 1 << REFS0 ) )
#define ADC_VCC_REF      ( ( 1 << REFS0 ) )

// VBAT computation
#define ADC_STEP        ((1.1f/1024.0f)*1000.0f)
#define ADC_R1          (100.0f)
#define ADC_R2          (51.0f)
#define ADC_R           (ADC_R1+ADC_R2)
#define ADC_K           ((ADC_STEP*ADC_R)/ADC_R2)
#define ADC_CAL_K       (ADC_STEP/3344.0f)            
#define ADC_CAL_REF     (2500.0f)

// BAND GAP VCC computation
// 1.1V * 1023 -> 1100*1023 = 1125300
#define ADC_VCC_K       1125300   
#define ADC_SAMPLES     4

/** @brief IO configuration
 *   IOS:
 *      PORTB:
 *          0 -> RADIO RST line  (OUTPUT def vlaue:1)
 *          1 -> RADIO CE line   (OUTPUT def value:0)
 *          2 -> RADIO SS line   (OUTPUT def value:1)
 *          3 -> RADIO MOSI line (OUTPUT def value:0)
 *          4 -> RADIO MISO line (INPUT no pull)
 *          5 -> RADIO SCLK line (OUTPUT def value:0)
 *          6 -> N/C (OUTPUT LOW)
 *          7 -> N/C (OUTPUT LOW)   
 *           DDRB=  0b1110 1111
 *           PORTB= 0b0000 0101
 *     PORTC:
 *          0 -> Buton (INPUT no pull) - PCINT8  is used to monotiro changes
 *          1 -> Wake  (OUTPUT def  HIGH)
 *          2 -> N/C   (OUTPUT def: LOW)
 *          3 -> Reed  (INPUT no pull) - PCINT11 is used to monitor changes
 *          4 -> SDA   (INPUT no pull)
 *          5 -> SCL   (INPUT no pull)
 *          6 -> RESET (INPUT no pull)     
 *          DDRC= 0b0000110
 *          PORTC=0b0000000
 *     PORTD:
 *          0-> RX (input no pull)
 *          1-> TX (output def: 0)
 *          2-> IRQ0 (INPUT ) RADIO   (no pull up)
 *          3-> IRQ1 (INPUT ) EXT INT (no pull up)
 *          4-> LED  (OUTPUT def: LOW)
 *          5-> DIO1  Lora Radio io (Input no pull)
 *          6-> DIO2  Lora Radio io (Input no pull)
 *          7-> N/C  (OUTPUT LOW)
 *          DDRD=  0b10010010
 *          PORTB= 0b00000000
 *      PORTE: (only for ATMEGA328PB)
 *          0-> FIXED GND (input no pull)
 *          1-> FIXED VCC (input no pull)
 *          3-> ADC6 NC   (ouput fixed to 0)
 *          4-> ADC7 NC   (input no pull)
 *          DDRE=  0b0100
 *          PORTE= 0b0000 
 *  PERIPHERICAL:
 *     TIMER0 -> Used by arduino millis
 *     TIMER1 -> Not used disabled
 *     TIMER2 -> Not used disabled
 *     SPI    -> Enabled RADIO
 *     UART0  -> Disabled in normal mode , enabled in config mode.
 *     I2C    -> Enabled only if BOROSREED24_I2C is defined
 *     ADC    -> Disabled by default, enabled when measuring
 *   
 *   ADC Considerations:
 *      ADC reference:          Internal 1.1V
 *      ADC clock prescaler:    1MHz-> 16  / 8Mhz-> 128
 *      ADC Noise reduction.
 *   ADC Calibration: ADC calibration is referenced to 2.5V AD586 voltage reference
 * 
 */
/** @brief Init IOs and tune the chip to lower power consumption
 *  
 */
void init_hw();
/** @brief turn on/off the on board led 
 *
 * @param value: 0=off / !0 = on
 */
void led(uint8_t value=0);
/** @brief get button status Button status 
 *
 *
 */
bool button(bool debounce=false);
/* @brief get button value in boolean
 * @param 
 * @return button value
 */
bool reed(bool inverted);
/**
 * @brief R
 * 
 * @param val 
 */
void wake(bool val);

/**
 * @brief 
 * 
 * @param adc_channel 
 * @return uint16_t 
 */
void measureADC(uint8_t adc_channel,uint8_t ref=ADC_INTERNAL_REF,uint8_t samples=ADC_SAMPLES);

/**
 * @brief 
 * 
 * @return uint16_t 
 */
uint16_t getADC();

/* @brief Measure batery voltage
 *
 */
int16_t MeasureVBat(uint8_t samples=ADC_SAMPLES);

/**
 * @brief 
 * 
 */
int16_t MeasureVcc(uint8_t samples=ADC_SAMPLES);

/** @brief enable_int1 setting pull up resistor
 *  
 */
 void enable_int1(bool val=false);

/**
 * @brief  enable_uart
 * 
 */
void enable_uart();

/** @brief deepSleep
 *
 *
 */
 enum wake_t {
     WAKE_WDT=0,
     WAKE_REED,
     WAKE_BUT,
     WAKE_EXTINT,
     WAKE_RADIO
 };

 wake_t deepSleep(bool forever=false);
 /**
  * @brief 
  * 
  * @return wake_t 
  */
 wake_t radioSleep();

/** @brief Reboot the mcu using WDT 
 */
 void reboot();
/**
 * @brief 
 * 
 * @param ms 
 */
 void patch_time(uint32_t ms);

#endif