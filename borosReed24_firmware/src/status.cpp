#include <EEPROM.h>
#include "status.hpp"
#include "dbg.h"

static const RConfig factory_conf PROGMEM = {
    MAGIK,
    false,                     ///< The reed is inverted
    false,                     ///< Blink on notification
    false,                     ///< Enable INT1 external wake up
    1,                         ///< Arbitrary device id (16bit unsigened integer)
    0,                         ///< Report multiplier (Report every n*8sec) if n=0 disebled
    0,                         ///< Accuracy byte
#if defined(BOROSREED24)
    "%Id,%Rd,%Bd,%AB,%Vd",     ///< payload template
    RF_PLAIN,                  ///< RF mode (PLAIN / MESH)
    {
    2,                         ///< PA Level (RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX)
    0,                         ///< Enable LNA 
    76,                        ///< RF Channel to use (1-127)
    0,                         ///< RF Speed to use (1Mbps,2Mbps,250K)
    5,                         ///< Address size in plain mode 3-5 
    {0x31,0x4e,0x6f,0x64,0x65},///< Remote to pipe to notify (Address).
    32,                        ///< Radio payload size in plain mode 1-32
    2,                         ///< CRC mode in plain mode
    true,                      ///< Auto-ack feature in plain mode
    8,                         ///< Number of retries 0-15
    6,                         ///< 250us*(n+1) delay  0-15 (min:250us,max:4ms)
    10,                        ///< Mesh node ID
    'D',                       ///< Mesh frame type
    0,                         ///< Mesh remote addr
    false                      ///< Force Mesh renew address allways after sleep
    }
#elif defined(BOROSREED_LORA)
    "%Rc%Bc%Ac%Vc",     ///< payload template in cayenne format
    RF_LORA_TTN,
    {
        {0,0,0,0,0,0,0,0},      ///< DevEUI LSB
        {0,0,0,0,0,0,0,0},      ///< appEUI LSB
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, ///< APPKEY MSB
        1,                                 ///< 1st port default 
        255,                               ///< No force channel
        255,                               ///< No force SF
        255,                               ///< No start SF on join.
    }
#endif
        
};



R24Status status;

void init_config(bool reset){
    uint16_t magik=get_cnfw(CGF_MAGIK);
    if(reset || magik != MAGIK){ // Reset factory defaults
        for(uint16_t i=0;i<sizeof(RConfig);i++) {
           EEPROM.update(BOROSREED_EEPROM_OFFSET+CGF_MAGIK+i,
                         pgm_read_byte(&( (const uint8_t *) &factory_conf )[i]) );
        }
    }
    // Load key configuration in status this avoid to read eeprom
    status.inverted=get_cnf(CFG_INVERTED);
    status.led=get_cnf(CFG_LED);
    status.enable_int=get_cnf(CFG_ENABLE_INT);
    status.id=get_cnfw(CFG_ID);
    status.report=get_cnfw(CFG_REPORT);
    status.accuracy=get_cnf(CFG_ACCURACY);
    get_config(CFG_PAYLOAD_TEMPLATE,status.payload_template,TEMPLATE_SIZE);
    status.rf_mode=(rf_mode_t)get_cnf(CFG_RF_MODE);
    #if defined(BOROSREED24)
        status.rf_ack=get_cnf(CFG_RF_ACK);
        #if defined(BOROSREED24_MESH)
        status.msh_frameid=get_cnf(CFG_MSH_FRAMEID);
        status.msh_daddr=get_cnf(CFG_MSH_DADDR);
        status.msh_force_renew=get_cnf(CFG_MSH_FORCE_RENEW);
        #endif
    #elif defined(BOROSREED_LORA)
        status.ttn_channel=get_cnf(CFG_TTN_CHANNEL);
        status.ttn_sf=get_cnf(CFG_TTN_SF);
        status.ttn_jsf=get_cnf(CFG_TTN_JSF);
        status.ttn_port=get_cnf(CFG_TTN_PORT);
    #endif
    

    // ADC Calibration constant
    //EEPROM.get(EEPROM.length()-1-sizeof(float),status.adc_cal);
}
/*
void save_adc_cal(float v) {
    EEPROM.put(EEPROM.length()-1-sizeof(float),v);
}*/

void get_config(config_par_t e,void *dest,uint16_t len) {
    uint8_t pos=BOROSREED_EEPROM_OFFSET+(uint8_t) e;
    uint8_t *d=(uint8_t *)dest;
    while(len>0){
        *d=EEPROM.read(pos);
        d++; pos++; len--;
    }

}
void save_config(config_par_t e,void *src,uint16_t len){
    uint8_t pos=BOROSREED_EEPROM_OFFSET+(uint8_t) e;
    uint8_t *s=(uint8_t *) src;
    while(len>0){
        EEPROM.update(pos,*s);
        s++; pos++; len--;
    }
}

void print_hex(Print &p,uint8_t* b,uint8_t l){
    char t[3];
    while(l>0){
        snprintf(t,3,"%02X",*b);
        p.print(t);
        b++; l--;
    }
}


void dump_config(Print &p){
    // p.print(F("Stored config, ")); p.print(sizeof(RConfig));
    //p.print(F(" of ")); p.println(EEPROM.length() - BOROSREED_EEPROM_OFFSET);
    p.print(F("[inv]Inv. reed:")); p.println(get_cnf(CFG_INVERTED));
    p.print(F("[led]Led:"));    p.println(get_cnf(CFG_LED));
    p.print(F("[eint]EInt enabled:")); p.println(get_cnf(CFG_ENABLE_INT));
    p.print(F("[id]DeviceID:")); p.println(get_cnfw(CFG_ID));
    p.print(F("[repo]Report(mul):")); p.println(get_cnfw(CFG_REPORT));
    p.print(F("[acc]Accuracy:")); p.println(get_cnf(CFG_ACCURACY));
    p.print(F("[mod]Rf Mode:")); p.println(get_cnf(CFG_RF_MODE));
    
    #if defined(BOROSREED24)
    uint8_t t=get_cnf(CFG_RF_ADDR_SIZE);
    p.print(F("[psz]Addr size:")); p.println(t);
    p.print(F("[pipe]Remote pipe:"));
    uint8_t addr[5]; 
    get_config(CFG_RF_DADDR,addr,t);
    print_hex(p,addr,t);
    p.println();
    p.print(F("[dsz]Payload size:")); p.println(get_cnf(CFG_RF_PAYLOAD_SIZE));
    p.print(F("[ack]AutoAck:")); p.println(get_cnf(CFG_RF_ACK));
    p.print(F("[crc]Crc:")); p.println(get_cnf(CFG_RF_CRC));
    p.print(F("[retr]Retry Count:")); p.println(get_cnf(CFG_RF_RETRY));
    p.print(F("[retd]Retry delay:")); p.println(get_cnf(CFG_RF_RETRY_DELAY));

    #if defined(BOROSREED24_MESH)
        p.print(F("[mnid]Mesh node  Id:")); p.println(get_cnf(CFG_MSH_NODEID));
        p.print(F("[mfid]Mesh frame type:")); p.println(get_cnf(CFG_MSH_FRAMEID));
        p.print(F("[mdst]Mesh Dest. node Id:")); p.println(get_cnf(CFG_MSH_DADDR));
        p.print(F("[mfor]Mesh force renew:")); p.println(get_cnf(CFG_MSH_FORCE_RENEW));
    #endif
    
    p.print(F("[txp]Tx power:")); p.println(get_cnf(CFG_RF_PA_LEVEL));
    p.print(F("[lna]Enable Lna:")); p.println(get_cnf(CFG_RF_ENABLE_LNA));
    p.print(F("[rate]Data rate:")); p.println(get_cnf(CFG_RF_SPEED));
    p.print(F("[cha]Channel:")); p.println(get_cnf(CFG_RF_CHANNEL));
    
    #elif defined(BOROSREED_LORA)
        uint8_t tmp[16];
        //p.println(F("[mode]:TTN"));
        get_config(CFG_TTN_DEVEUI,tmp,8);
        p.print(F("[dui]DevEUI:"));   print_hex(p,tmp,8);
        get_config(CFG_TTN_APPEUI,tmp,8); 
        p.print(F("\n[aui]AppEUI:")); print_hex(p,tmp,8);
        get_config(CFG_TTN_APPKEY,tmp,16);
        p.print(F("\n[apk]AppKey:"));  print_hex(p,tmp,16);
        p.print(F("\n[cha]Channel:")); p.println(get_cnf(CFG_TTN_CHANNEL));
        p.print(F("[por]Port:")); p.println(get_cnf(CFG_TTN_PORT));
        p.print(F("[sf]SF:")); p.println(get_cnf(CFG_TTN_SF));
        p.print(F("[jsf]Join SF:")); p.println(get_cnf(CFG_TTN_JSF));
    #endif
    p.print(F("[tpl]Payload template:"));
    get_config(CFG_PAYLOAD_TEMPLATE,status.payload_template,TEMPLATE_SIZE);
    p.print(status.payload_template);
    //p.println(F(">>"));
    p.println();

}