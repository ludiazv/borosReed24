#include "notify.hpp"
#include "cayennelpp.h"


#if defined(BOROSREED_LORA)
static uint32_t _timer_patch;
#endif

static  int16_t _report;
static  uint8_t  _payload_size;
static  uint8_t  _cayenne_channel;


bool add_cayenne(uint8_t type,uint16_t data) {

    uint8_t size;
    if(type==LPP_DIGITAL_INPUT || type==LPP_PRESENCE || type==LPP_RELATIVE_HUMIDITY) {
        size= LPP_DIGITAL_INPUT_SIZE;
        if(type==LPP_RELATIVE_HUMIDITY) data=data/50;  // patch mudity data to cayenne 0.5% precission
    } else if(type==LPP_ANALOG_INPUT || type==LPP_TEMPERATURE ||
              type==LPP_BAROMETRIC_PRESSURE) {
        size= LPP_ANALOG_INPUT_SIZE;
    } else return false;
    if((_payload_size + size)> BUFFER_SIZE) return false;
    status.buffer[_payload_size++]=_cayenne_channel++;
    status.buffer[_payload_size++]=type;
    if(size==LPP_DIGITAL_INPUT_SIZE) status.buffer[_payload_size++]=(uint8_t) data;
    else {
        int16_t v= data / 10; //(mV to cV signed o Temp cayenne LPP)
        status.buffer[_payload_size++]= v >> 8;
        status.buffer[_payload_size++]= (uint8_t) v;
    } 
    return true;
}

bool add_fragment(const char *frag){

    int n=snprintf((char *)(status.buffer+_payload_size),BUFFER_SIZE-_payload_size,frag);
    _payload_size+=n;
    if(_payload_size>BUFFER_SIZE) {
        _payload_size=BUFFER_SIZE;
        status.buffer[_payload_size]='\0';
        return false;
    }
    status.buffer[_payload_size]='\0';
    return true;
}

void add_binary(const char*bin,uint8_t size) {
    if((_payload_size+size) > BUFFER_SIZE) return;
    while(size > 0){
        *(status.buffer+_payload_size)=*bin;
        bin++; size--;
        _payload_size++;
    }
    status.buffer[_payload_size]='\0';
}

void add_variable(char v,char f) {
    // Variables:
    // I: ID 16-bit
    // i: ID 8-bit
    // R: reed status           - bool
    // V: VCC    (16bit)        - milivolts  
    // B: Batery (16bit)        - milivolts
    // T: Temp   (16bit)        - 0.1C
    // H: Humidity (16bit)
    // A: Battery alarm         - bool 
    uint16_t data;
    bool _8bit=(v=='i' || v=='R' || v=='A');
    uint8_t cayenne=255;
    if(v=='I' || v=='i') {
        data=status.id;
    } else if(v=='R') {
        data=status.reed;
        cayenne= LPP_PRESENCE;
    } else if(v=='B') {
        data=status.batery;
        cayenne= LPP_ANALOG_INPUT;
    } else if(v=='A') {
        data=status.batery <= VBAT_ALARM;
        cayenne= LPP_DIGITAL_INPUT;  
    } else if(v=='V') {
        data=status.vcc;
        cayenne= LPP_ANALOG_INPUT;
    } else if(v=='T') {
        data=status.temp;
        cayenne= LPP_TEMPERATURE;
    } else if(v=='H') {
        data=status.hum;
        cayenne= LPP_RELATIVE_HUMIDITY;
    } else if(v=='P') {
        data=status.preasure;
        cayenne= LPP_BAROMETRIC_PRESSURE;
    } else return;
    // Formats:
    // d: decimal 
    // x: hexadecimal string
    // b: binary LSB
    // B: bool-> "true"/"false"
    // c: Cayenne
    char tmp[6];
    char format[6]={'%','0','4','X',0,0};
    if(f=='d') {
        format[1]='d'; format[2]='\0';
    } else if(f=='x') {
        if(_8bit) format[2]='2';
    } else if(f=='b') {
        format[0]='\0';
    } else if(f=='c') {
         format[0]='c'; // Mark cayenne format
    } else if(f=='B') {
        if(data) {
            format[0]='t'; format[1]='r'; format[2]='u';
            format[3]='e'; //format[4]='\0';
        } else {
            format[0]='f'; format[1]='a'; format[2]='l';
            format[3]='s'; format[4]='e'; //format[5]='\0';
        }
    }
    // OUTPUT
    if(format[0]=='\0') { // Binary
        add_binary((const char*)&data,(_8bit) ? 1: 2);      
    } else if(format[0]=='c') {
        add_cayenne(cayenne,data);
    } else {
        snprintf(tmp,6,format,data);
        add_fragment(tmp);
    }

}

void prepare_payload(){
    _payload_size=0;
    _cayenne_channel=1;
    char *p=status.payload_template,*m;
    char *end=status.payload_template + strlen(status.payload_template)-1;
   
    while(*p!='\0' && p<=end){
       
        if((m=strchr(p,'%'))!=NULL) { // marker found
            if(p!=m) {
                *m='\0';
                if(!add_fragment(p)) {
                    *m='%';
                    break;
                }
                *m='%';
            }
            if(*(m+1)=='%') {
              add_fragment("%%"); // Escape %
              p=m+2;
              continue;
            }
            if(*(m+1)!='\0' && *(m+2)!='\0') {
              add_variable(*(m+1),*(m+2));
              p=m+3;      
            } else break;

        } else { // last fragment
            add_fragment(p);
            break;
        }

    }

}

void do_notification(){
    
    // ----------------------
    // - Notification phase 
    // ----------------------
    led(status.led);
    DBG_PRINTLN(F("Notify..."));
    DBG_FLUSH();
    // Sense the status
    sense(DBG_EXP(true));
    // --- payload preparation
    DBG_PRINT(F("[payload"));
    DBG_FLUSH();
    prepare_payload();
    DBG_EXP(status.buffer[_payload_size]='\0');
    DBG_PRINT(F("size]:"));
    DBG_PRINTLN(_payload_size);
    DBG_FLUSH();
    // --- Notification section ----
    if(_payload_size>0) {
        radio_pwr(true); // Enable the radio
        radio_connect();
        radio_send(status.buffer,_payload_size);
        radio_disconnect();
    }
    radio_pwr(false);           // Disable radio
    if(status.led) led(false);  // Disable led after notification

    // short notification periodo if the reed chaged just 
    // after sending wait 4 seconds and send notify again
    // --- This helps when mesh / Lora is used ----
    bool reed_after=reed(status.inverted);
    if(reed_after!=status.reed) {
        led(status.led);    // Keep led in waiting
        status.reed=reed_after;
        idle(SLEEP_4S);
        //delay(4000);
        return;
    }

    // ----------------
    // Sleep phase
    // ----------------
    DBG_PRINTLN(F("[sleep]"));
    DBG_FLUSH();
    _report= (status.report==0) ? 1 : status.report;
    // sleep section
    wake_t w;
    #if defined(BOROSREED_LORA)
    _timer_patch=0;
    #endif
    do {
        #if defined(BOROSREED_LORA)
            w=deepSleep(false);
            _timer_patch++;
            if(status.report==0) _report=2;
        #else
            w=deepSleep(status.report==0);
        #endif
        _report--;

    } while(w==WAKE_WDT && _report>0);

    status.reed=reed(status.inverted);
    DBG_PRINT(F("[w/reed]:"));
    DBG_PRINT(status.reed);
    DBG_PRINTLN(w);
    DBG_FLUSH();
    if(w==WAKE_BUT) reboot();
    
    #if defined(BOROSREED_LORA)
        patch_time(TOTAL_8S_MS*_timer_patch);
    #endif

}