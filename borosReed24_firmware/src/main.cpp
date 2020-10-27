
#include "version.h"
#include "notify.hpp"
#include "config.hpp"

#define MODE_NOTIFY 0
#define MODE_CONFIG 1
#define MODE_ERROR  2

uint8_t current_mode;

void setup() {
    // HAL setup (pins, power reduction,etc)
    init_hw();
    bool but=button(true);
    if(but) {
        enable_uart();
        idle(SLEEP_120MS);
        //delay(100);
        Serial.begin(DBG_SPEED);
    } else {
        DBG_EXP(enable_uart());
        DBG_EXP(idle(SLEEP_120MS));
        //delay(100);
        DBG_INIT(); // Enable debug if defined
        (void) but;
    }
    
    // load/init config
    init_config(false);
    // Init the radio (will be powered off)
    bool radio_ok=radio_init();
    //bool radio_ok=true;
    // Init the sensors
    bool sensor_ok=sense_init();
    //bool sensor_ok=true;

    // Check config to set the mode.
    if(but) {
        Serial.println(F("\nConfiguration mode"));
        Serial.print(F("SOK:")); Serial.println(sensor_ok);
        Serial.print(F("ROK:")); Serial.println(radio_ok);
        current_mode=MODE_CONFIG;
        print_banner();
        Serial.flush();
        led(true);
    } else {
        DBG_PRINTLN(F("\nNotification mode"));
        DBG_PRINT(F("SOK:")); DBG_PRINTLN(sensor_ok);
        DBG_PRINTLN(F("Connecting..."));
        radio_ok=radio_ok && radio_connect();
        DBG_PRINT(F("ROK:")); DBG_PRINTLN(radio_ok);
        DBG_FLUSH();
        current_mode= (radio_ok && sensor_ok) ? MODE_NOTIFY : MODE_ERROR;
        #if defined(BOROSREED_LORA)
            idle(SLEEP_4S); // Helps with single channel gateways after OTAA
        #endif
    }
    // read reed status
    status.reed=reed(status.inverted);
    //DBG_EXP(dump_config(Serial));
    //DBG_FLUSH();
}

void loop() {

    if(current_mode == MODE_NOTIFY) do_notification();
    else if(current_mode == MODE_CONFIG) {
        do_config();
        if(!button(true)) {
            Serial.println(F("Mode changed"));
            Serial.flush();
            reboot();
        }
    } else { // Error mode
        DBG_PRINTLN(F("ERROR"));
        led(true);
        deepSleep(SLEEP_2S);
        led(false);
        deepSleep(SLEEP_2S);
        if(button(true)) reboot();
    }

}