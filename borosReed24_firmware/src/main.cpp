
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
        delay(100);
        Serial.begin(DBG_SPEED);
    } else {
        DBG_EXP(enable_uart());
        DBG_EXP(delay(100));
        DBG_INIT(); // Enable debug if defined
        (void) but;
    }
    
    // load/init config
    init_config(true);
    // Init the radio (will be powered off)
    bool radio_ok=radio_init();
    //bool radio_ok=true;
    
    // Check config to set the mode.
    if(but) {
        Serial.println(F("\nBoot in configuration mode [" BOROSREED24_VERSION "]"));
        Serial.print(F("ROK:")); Serial.println(radio_ok);
        current_mode=MODE_CONFIG;
        print_banner();
        Serial.flush();
        led(true);
    } else {
        DBG_PRINTLN(F("\nBoot in notification mode [" BOROSREED24_VERSION "]"));
        DBG_PRINTLN(F("Connecting..."));
        radio_ok=radio_ok && radio_connect();
        DBG_PRINT(F("ROK:")); DBG_PRINTLN(radio_ok);
        DBG_FLUSH();
        current_mode= (radio_ok) ? MODE_NOTIFY : MODE_ERROR;
        #if defined(BOROSREED_LORA)
            delay(2000); // Helps with single channel gateways after OTAA
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
        led(true);
        delay(3000);
        led(false);
        delay(3000);
        if(button(true)) reboot();
    }

}