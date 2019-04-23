#include "sensor.hpp"

#if defined(BOROSMET_SHTC3)
    #include <SHTC3.h>
    static SHTC3 _shtc3(Wire);
#endif


bool sense_init(bool verbose) {
    bool r=true;
    #if defined(BOROSMET_SHTC3)
        r=_shtc3.begin(false);
    #endif
    return r;
}

void sense(bool verbose){

    // --- Measure ADC ----
    wake(false);  // Enable wake signal for batery measure 
    delay(5);
    status.batery=MeasureVBat();
    if(verbose) {
        Serial.print(F("[VBAT](mV):"));
        Serial.println(status.batery);
        Serial.flush();
    }
    wake(true);   // Disable batery measurement mosfet
    status.vcc=MeasureVcc();
    if(verbose) {
        Serial.print(F("[VCC](mV):"));
        Serial.println(status.vcc);
    }
    // -----------------------

    #if defined(BOROSMET_SHTC3)
        if(_shtc3.sample((status.accuracy) ? SHTC3_READ : SHTC3_READ_LP)) {
            status.temp= _shtc3.readTempC();
            status.hum = _shtc3.readHumidity();
            if(verbose) {
                Serial.print(F("SHTC3[TEMP](cC):"));
                Serial.println(status.temp);
                Serial.print(F("SHTC3[HUM](c%):"));
                Serial.println(status.hum);                
            }
        }
    #endif

    if(verbose) Serial.flush();
}