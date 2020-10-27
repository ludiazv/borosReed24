#include "config.hpp"
#include "version.h"


static const char banner[] PROGMEM = "BorosReed24 config console [" BOROSREED24_VERSION "] Type 'help' to show commands";
static const char help[] PROGMEM = "---COMMANDS---:\n"
    "Misc:\n"
    "=====\n"
    "help - shows this msg\n"
    "ver  - shows version banner\n"
    "show - shows current configuration\n"
    "sen  - shows sensors status\n"
    "rebo - reboot device\n" 
    "fac  - override config with factory defaults\n"
    "General:\n"
    "========\n"
    "inv <1/0>   - invert reed sensor reading\n"
    "led <1/0>   - blink led on notification\n"
    "eint <1/0>  - enable external interrupt\n"
    "id <number> - set 16bit integer ID\n"
    "repo <mul>  - set periodic report = 8*mul seconds.(0=disabled)\n"
    "acc <n>     - set accuracy byte\n"
    "tpl <template> - set payload template\n"
    "mode <0-3> - set radio mode 0=plain,1=mesh,2=lorawan\n"
#if defined(BOROSREED24)
    "Radio:\n"
    "======\n"    
    "txp <0-3>   - set TX power level\n"
    "lna 0/1     - enable lna\n"
    "cha <1-126> - set radio channel\n"
    "rate <0/1/2> - set radio data rate 0=1MBPS,1=2MBPS,3=250KBPS\n"    
    "Plain mode:\n"
    "===========\n"
    "psz  <3/4/5> - set pipe address size\n"
    "pipe <hex>   - set hex destiantion pipe (LSB)\n"
    "dsz  <0-32>  - set data/payload size\n"
    "crc  <0/1/2> - set crc 0=disabled,1=8bit,2=16bit\n"
    "ack  <0/1>   - Use autoack\n"
    "retr <0-15>  - set number of tx retries\n"
    "retd <0-15>  - set retries delay (n+1)*250us\n"
    "Mesh mode:\n"
    "==========\n"
    "mnid <1-255> - set mesh node ID\n"
    "mfid <0-255> - set mesh frame type\n"
    "mdst <0-255> - set mesh node destination id\n"
    "mfor <0/1>   - force mesh renew connection"
#elif defined(BOROSREEED_LORA)
    "TTN info:\n"
    "=========\n"
    "dui <hex>    - TTN  DEVEUI(LSB)\n"
    "aui <hex>    - TTN  APPEUI(LSB)\n"
    "apk <hex>    - TTN  APPKEY(MSB)\n"
    "por <number> - port number\n"
    "cha <number> - TTN channel number\n"
    "sf  <7-12>   - Spread Factor\n"
    "jsf <7-12>   - SF on join"
#endif
    "";

static const Boros::command_t cmds[] PROGMEM={
    // HELP & BANNER
     "help",
     "ver",
     "show",
    // COMMANDS
     "sen",
     "rebo",
     "fac",
    // COMMON CONFIG
     "inv",
     "led",
     "eint",
     "id",
     "repo",
     "acc",
     "tpl",     
     "mode",
     // Radio specific commands
#if defined(BOROSREED24)
     "txp",
     "lna",
     "cha",
     "rate",
     "psz",
     "pipe",
     "dsz",
     "crc",
     "ack",
     "retr",
     "retd",
   #if defined(BOROSREED24_MESH)  
     "mnid",
     "mfid",
     "mdst",
     "mfor",
   #endif
#elif defined(BOROSREED_LORA)
     "dui",
     "aui",
     "apk",
     "por",
     "cha",
     "sf",
     "jsf"
#endif
     // SPECIAL CALIBRATION COMMAND
     //"cal"  
};


enum cmd_ids_t {
    // HELP & BANNER
    CMD_HELP=1,
    CMD_VER,
    CMD_SHOW,
    // COMMANDS
    CMD_SEN,
    CMD_REBO,
    CMD_FAC,
    // COMMON CONFIG
    CMD_INV,
    CMD_LED,
    CMD_EINT,
    CMD_ID,
    CMD_REPO,
    CMD_ACC,
    CMD_TPL,     
    CMD_MODE,
#if defined(BOROSREED24)
    CMD_TXP,
    CMD_LNA,
    CMD_CHA,
    CMD_RATE,
    CMD_PSZ,
    CMD_PIPE,
    CMD_DSZ,
    CMD_CRC,
    CMD_ACK,
    CMD_RETR,
    CMD_RETD,
    #if defined(BOROSREED24_MESH)
    CMD_MNID,
    CMD_MFID,
    CMD_MDST,
    CMD_MFOR,
    #endif

#elif defined(BOROSREED_LORA)
    CMD_DUI,
    CMD_AUI,
    CMD_APK,
    CMD_POR,
    CMD_CHA,
    CMD_SF,
    CMD_JSF,
#endif
    //CMD_CAL
};

#define n_cmds  ((uint8_t)(sizeof(cmds)/sizeof(Boros::command_t)))


Boros::Console con(cmds,n_cmds,(char *)status.buffer,BUFFER_SIZE);
Boros::ConsoleCmd cmd;

void print_banner() {
    Serial.println(FH(banner));
}

void do_config() {

    uint8_t cm=con.loop(&cmd);
    if(cm==0) return; // No command
    bool ok=true;
    bool rebot=false;
    bool    b;
    uint8_t u8;
    uint16_t u16;
    #if defined(BOROSREED24)
    uint8_t  tmp[5];
    #elif defined(BOROSREED_LORA)
    uint8_t  tmp[16];
    #endif

    if(cm==1) {
        bool rst=true;
        switch(cmd.cmd){
            case CMD_HELP:
                Serial.println(FH(help));
                break;
            case CMD_VER:
                Serial.println();
                print_banner();
                break;
            case CMD_SEN:
                sense(true);
                break;
            case CMD_REBO:
                rebot=true;
                break;
            case CMD_FAC:
                init_config(true);
                rebot=true;
                break;
            case CMD_SHOW:
                Serial.println();
                dump_config(Serial);
                break;
            /*case CMD_CAL:
                wake(false);  // Enable batery measure
                delay(100);
                Serial.print(ADC_CAL_REF,1);
                Serial.print(F("mV cal/Raw:"));
                measureADC(VBAT_ADC,ADC_INTERNAL_REF,8);
                {
                    uint16_t raw=getADC();
                    float k= ADC_CAL_REF / (float)raw;
                    Serial.print(raw);
                    save_adc_cal(k);
                    Serial.print(F("/K:"));
                    Serial.println(k,6);
                }
                rebot=true;
                break;*/
            default:
                rst=false;
                return;
        }
        if(rst) con.reset();

    } else if(cm==2) {
        b=con.parse_bool(&cmd);
        u8=con.parse_uint8(&cmd);
        switch(cmd.cmd) {
            case CMD_INV:
                set_cnf(CFG_INVERTED,b);
                break;
            case CMD_LED:
                set_cnf(CFG_LED,b);
                break;
            case CMD_EINT:
                set_cnf(CFG_ENABLE_INT,b);
                break;
            case CMD_ID:
                set_cnfw(CFG_ID,con.parse_uint16(&cmd));
                break;
            case CMD_REPO:
                set_cnf(CFG_REPORT,con.parse_uint16(&cmd));
                break;
            case CMD_ACC:
                set_cnf(CFG_ACCURACY,u8);
                break;
            case CMD_TPL:
                u16=strlen(cmd.param);
                if((ok=(u16>0 && u16<TEMPLATE_SIZE))) save_config(CFG_PAYLOAD_TEMPLATE,cmd.param,u16+1);
                break;
            case CMD_MODE:
                #if defined(BOROSREED24)
                    #if defined(BOROSREED24_MESH)
                    if((ok=(u8==0 || u8==1))) set_cnf(CFG_RF_MODE,u8);
                    #else
                       if((ok=(u8==0))) set_cnf(CFG_RF_MODE,u8);
                    #endif
                #elif defined(BOROSREED_LORA)
                    if((ok=(u8==2))) set_cnf(CFG_RF_MODE,u8);
                #endif
                break;
                
            #if defined(BOROSREED24)
           
            case CMD_TXP:
                if((ok=(/*u8>=0 && */u8<=3))) set_cnf(CFG_RF_PA_LEVEL,u8);
                break;
            case CMD_LNA:
                set_cnf(CFG_RF_ENABLE_LNA,b);
                break;
            case CMD_CHA:
                if((ok=(u8>0 && u8<127))) set_cnf(CFG_RF_CHANNEL,u8);
                break;
            case CMD_RATE:
                if((ok=(/*u8>=0 &&*/ u8<=2))) set_cnf(CFG_RF_SPEED,u8);
                break;
            case CMD_PSZ:
                if((ok=(u8>=3 && u8<=5))) set_cnf(CFG_RF_ADDR_SIZE,u8);
                break;
            case CMD_PIPE:
                u8= get_cnf(CFG_RF_ADDR_SIZE);
                if((ok=con.parse_hex(&cmd,tmp,u8))) save_config(CFG_RF_DADDR,tmp,u8);
                break;
            case CMD_DSZ:
                if((ok=(u8>0 && u8<=32))) set_cnf(CFG_RF_PAYLOAD_SIZE,u8);
                break;
            case CMD_CRC:
                if((ok=(u8<=2))) set_cnf(CFG_RF_CRC,u8);
                break;
            case CMD_ACK:
                set_cnf(CFG_RF_ACK,b);
                break;
            case CMD_RETR:
                if((ok=(u8<=15))) set_cnf(CFG_RF_RETRY,u8);
                break;
            case CMD_RETD:
                if((ok=(u8<=15))) set_cnf(CFG_RF_RETRY_DELAY,u8);
                break;

            #if defined(BOROSREED24_MESH)

            case CMD_MNID:
                if((ok=(u8>0))) set_cnf(CFG_MSH_NODEID,u8);
                break;
            case CMD_MFID:
                if((ok=(u8>=1 && u8<=127))) set_cnf(CFG_MSH_FRAMEID,u8);
                break;
            case CMD_MDST:
                if((ok=(u8!=get_cnf(CFG_MSH_NODEID)))) set_cnf(CFG_MSH_DADDR,u8);
                break;
            case CMD_MFOR:
                set_cnf(CFG_MSH_FORCE_RENEW,b);
                break;

            #endif

            #elif defined(BOROSREED_LORA)
            case CMD_DUI:
            case CMD_AUI:
                if((ok=con.parse_hex(&cmd,tmp,8))) {
                    if(cmd.cmd==CMD_DUI) save_config(CFG_TTN_DEVEUI,tmp,8); 
                    else save_config(CFG_TTN_APPEUI,tmp,8);
                }
                break;
            case CMD_APK:
                if((ok=con.parse_hex(&cmd,tmp,16))){
                    save_config(CFG_TTN_APPKEY,tmp,16);
                }
                break;
            case CMD_CHA:
                set_cnf(CFG_TTN_CHANNEL,u8);
                ok=true;
                break;
            case CMD_POR:               
                if((ok=(u8>0))) set_cnf(CFG_TTN_PORT,u8);
                break;
            case CMD_SF:
            case CMD_JSF:
                if(u8<7 || u8>12) u8=255;
                if(cmd.cmd==CMD_SF) set_cnf(CFG_TTN_SF,u8); else set_cnf(CFG_TTN_JSF,u8);
                ok=true;
                break;
            #endif
            default:
                ok=false;
        }
        con.reset();   

    } else ok=false;
   
    if(ok) Serial.println(F("\n[OK]\n")); else Serial.println(F("\n[ERROR]\n"));
    if(rebot) {
        Serial.println(F("Reboot!"));
        Serial.flush();
        reboot();
    }
    Serial.flush();
}