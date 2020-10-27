#ifdef BOROSREED_LORA
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "radio_lora.hpp"


// Configuration pins Arduino IOS
#define CS_PIN          10
#define RST_PIN          8
#define DIO0_PIN         2
#define DIO1_PIN         5
#define DIO2_PIN         6


// Constants
// Max send to ten seconds
#define MAX_SEND_TIMEOUT  10000
// Set max join to 10 minutes
#define MAX_JOIN_TIMEOUT  600000L

#if defined(CFG_eu868)
#define LM_MAX_CHANNEL      9
#elif defined(CFG_us915)
#define LM_MAX_CHNNEL       71
#else
#error "Band plan not supportted yet"
#endif

const lmic_pinmap lmic_pins = {
    .nss = CS_PIN,                      
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RST_PIN,                      
    .dio = {DIO0_PIN, DIO1_PIN, DIO2_PIN}
};

//static osjob_t _sendjob;
enum send_status_t { SEND_NONE=0, SEND_START, SEND_DONE };
static send_status_t _send_status;
enum join_status_t { JOIN_NONE=0, JOIN_JOINING, JOIN_FAIL, JOIN_DONE };
static join_status_t _join_status;
static uint32_t _join_started;

/* LMIC Callabcks */
#ifndef DISABLE_JOIN
//static const u1_t PROGMEM APPEUI[8]= { 0x68, 0x4A, 0x01, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
//void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getArtEui (u1_t* buf) { get_config(CFG_TTN_APPEUI,buf,8); }
//static const u1_t PROGMEM DEVEUI[8]={ 0x65, 0x2F, 0x3E, 0x68, 0xD6, 0xE6, 0xA4, 0x00 };
//void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevEui (u1_t* buf) { get_config(CFG_TTN_DEVEUI,buf,8); }
//static const u1_t PROGMEM APPKEY[16] = { 0xE4, 0xF2, 0xE9, 0x00, 0x4C, 0x08, 0x1A, 0x1F, 0xE8, 0xFF, 0x42, 0x9D, 0x1D, 0x68, 0x4C, 0xF5 };
//void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16); }
void os_getDevKey (u1_t* buf) {  get_config(CFG_TTN_APPKEY,buf,16); }
#endif

static const uint8_t DR_CODES[6] PROGMEM = { DR_SF7, DR_SF8, DR_SF9 , DR_SF10, DR_SF11, DR_SF12};
void setupChannelDR(uint8_t ch,uint8_t DR){
     // Disable link check validation & ADR
    LMIC_setLinkCheckMode(0);
    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;
    if(ch<LM_MAX_CHANNEL) {
        for(uint8_t i=0;i<LM_MAX_CHANNEL;i++) 
            if(i!=ch) LMIC_disableChannel(i);
        DBG_PRINT(F(" FCH:"));
        DBG_PRINT(ch);
    }
    if(DR>=7 && DR<=12) {
        dr_t dr=(dr_t) pgm_read_byte(&DR_CODES[DR-7]);
        // Power is ignored by the librare.
        LMIC_setDrTxpow(dr, 20);
        DBG_PRINT(F("/FSF:"));DBG_PRINT(DR);DBG_PRINT('#');DBG_PRINTLN(dr);
    }
    DBG_PLN();
    DBG_FLUSH();
}

void onEvent (ev_t ev) {
  uint32_t mil=millis();
  DBG_PRINT(mil);
  if(ev==EV_TXCOMPLETE) {
      DBG_PRINTLN(F(":TXD"));
      _send_status=SEND_DONE;
  } else if(ev==EV_TXSTART) {
    DBG_PRINT(F(":TX-"));DBG_PRINT(LMIC.freq);DBG_PRINT('/');DBG_PRINTLN(LMIC.datarate);
    if(_join_status==JOIN_DONE) _send_status=SEND_START;
    else _join_started=mil;
  } else if(ev==EV_JOINING){
    DBG_PRINT(F(":JG-"));  
    setupChannelDR(255,status.ttn_jsf); // Configure chanel but only forcing Channel  
    _join_status=JOIN_JOINING;
  } else if(ev==EV_JOINED) {
    DBG_PRINTLN(F(":JO-"));
    setupChannelDR(status.ttn_channel,status.ttn_sf);
    _join_status=JOIN_DONE;
  } else if(ev==EV_JOIN_FAILED) {
    DBG_PRINTLN(F(":JF"));
    _join_status=JOIN_FAIL;
  } else {
    DBG_PRINT(F(":EV "));
    DBG_PRINTLN(ev);
    (void) ev;
  }
  DBG_FLUSH();

}

void radioL_loop(uint32_t ms){
    os_runloop_once();
}

bool radioL_init(){

    // LORA Init
    os_init();
    // Reset Radio
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 20 / 100);  // BIG INITIAL CLOCK ERROR to deal with OTAA

    //Set up Lmic ABP session.
    /*devaddr_t dev_addr=0x2601100F;
    uint8_t nwKey[16]={ 0x95, 0x69, 0x4E, 0x71, 0x1D, 0x05, 0x1E, 0x01, 0x30, 0x14, 0xD9, 0x1C, 0x9C, 0xE3, 0xE1, 0x29 };
    uint8_t aKey[16]= { 0x8E, 0xE1, 0x46, 0x4E, 0x54, 0xA5, 0x65, 0xEC, 0xAE, 0xE8, 0x2F, 0x96, 0xE9, 0x51, 0xDB, 0xAB };
    LMIC_setSession(NETID,dev_addr,nwKey,aKey);*/

    // Set up bands
#if defined(CFG_eu868)
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
#elif defined(CFG_us915)
  // NA-US channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#endif
    // Force channels & DR
    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;
    _join_status=JOIN_NONE;
    return true;
}

bool radioL_connect(){
    if(_join_status==JOIN_DONE) return true;
    if(_join_status==JOIN_NONE) {
        led(true); 
        _join_started=0;
        LMIC_startJoining();
        if(status.ttn_channel< LM_MAX_CHANNEL) LMIC.txChnl = status.ttn_channel; // First channel tried
        while(_join_started==0) os_runloop_once();
        led(false);
    }
    // JOINIG SECTION
    os_runloop_once();
    uint32_t started=millis(),mils;
    while(_join_status<=JOIN_JOINING) {
        os_runloop_once();
        mils=millis();
        if((mils-started)> MAX_JOIN_TIMEOUT) break;
        if(mils % 1000 > 500) led(true); else led(false);
    }
    led(false);
    LMIC_setClockError(MAX_CLOCK_ERROR / 500); // Reduce clock error when connected via OTAA to speed up TX & save power
    os_runloop_once();
    return _join_status == JOIN_DONE;
}

void radioL_disconnect() {
    os_runloop_once();
}

void radioL_pwr(bool on) {
    os_runloop_once();
}

bool radioL_send(const void* payload,uint8_t size){
    _send_status=SEND_NONE;
    if (LMIC.opmode & OP_TXRXPEND) {
        DBG_PRINTLN(F("PTX-ABORT"));
       return false;
    } else {
        os_runloop_once();
        LMIC_setTxData2(1, (u1_t *)payload, size, 0);
    }
    uint32_t started=millis();
    while(_send_status<=SEND_START) {
        os_runloop_once();
        if((millis()-started)> MAX_SEND_TIMEOUT) break;
    }
    return _send_status == SEND_DONE;
}

#endif