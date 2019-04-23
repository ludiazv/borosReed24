#ifndef _H_BOROSREED24_STATUS_H_
#define _H_BOROSREED24_STATUS_H_

#include<Arduino.h>


/**
 * @brief Support RF modes
 *  PLAIN         -> PLAIN RF24 pipes
 *  MESH          -> Mesh network based on nrf24Mesh protocol
 *  LORA_TTN      -> TTN LoRaWan
 *  LORA_PLAIN    -> Plain Lora / FSK (planned)
 *  RMF69         -> Future support   (planned support)
 */
enum rf_mode_t {
    RF_PLAIN=0,
    RF_MESH,
    RF_LORA_TTN,
    RF_LORA_PLAIN,
    RF_RFM69
};

/**
 * @brief Configuration of the device stored in EEPROM. The configuration
 * will persist.
 * 
 *  Common configuration is the following:
 *  - inverted: false
 *  - enable_int: false
 *  - id: 0x0001
 *  - report: 0
 *  - template: '%Is,%Rs,%Ts,%Bs,%As'
 *  - rf_mode: Plain 
 
 *   RF section:
 *  - rf_mode: Plain
 *  - rf_PA_level: RF24_PA_HIGH
 *  - rf_channel: 76
 *  - rf_speed: 1MBPS
 *  - rf_template: '%Is,%Rs,%Ts,%Bs,%As'

 *  Plain mode parameters: (This parameters are ignored if rf mode = mesh)
 *  - rf_addr_size: 5
 *  - rf_daddr: "0xAB11
 *  - rf_playload_size: 32
 *  - rf_crc: 16bit
 *  - rf_ack: true
 *  - rf_retry: 8
 *  - rf_retry_delay: 6
 *
 * 
 *  Mesh parameters: (This parameters are ingnored if rf mode = plain)
 *  - msh_nodeid: 10           
 *  - msh_frame_id: 'D'
 *  - msh_daddr: 0
 *  - msh_force_renew: Force allways to renew mesh connection
 * 
 */

#if defined(BOROSREED24)
    #if defined(BOROSREED24_MESH)
        #define BR_MAX_FRAME_SIZE  65
    #else
        #define BR_MAX_FRAME_SIZE  32
    #endif
    #define MAGIK 0xABCA
    struct RadioConfig {
        /// Common for all Radio mode
        uint8_t  rf_PA_level;     ///< PA Level (RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX)
        uint8_t  rf_channel;      ///< RF Channel to use (1-127)
        uint8_t  rf_speed;        ///< RF Speed to use (1Mbps,2Mbps,250K)
        /// Plain parameters
        uint8_t  rf_addr_size;    ///< Address size in plain mode 3-5
        uint8_t  rf_daddr[5];     ///< Remote to pipe to notify (Address).
        uint8_t  rf_payload_size; ///< Radio payload size in plain mode 0-32. 0= Dynamic payload
        uint8_t  rf_crc;          ///< CRC mode in plain mode
        bool     rf_ack;          ///< Auto-ack feature in plain mode
        uint8_t  rf_retry;        ///< Number of retries 0-15
        uint8_t  rf_retry_delay;  ///< 250us*(n+1) delay  0-15 (min:250us,max:4ms)
            /// Mesh parameters
        uint8_t  msh_nodeid;      ///< Mesh node ID
        uint8_t  msh_frameid;     ///< Mesh frame type
        uint8_t  msh_daddr;       ///< Mesh remote addr
        bool     msh_force_renew;  ///< Force Mesh renew address allways after sleep
    };
#elif defined(BOROSREED_LORA)
    #define MAGIK 0xAB21
    #define BR_MAX_FRAME_SIZE  50
    struct RadioConfig {
        uint8_t  ttn_devEUI[8];
        uint8_t  ttn_appEUI[8];
        uint8_t  ttn_appKey[16];
        uint8_t  ttn_port;
        uint8_t  ttn_channel;
        uint8_t  ttn_sf;
        uint8_t  ttn_jsf;
    };
#endif

#define BUFFER_SIZE      BR_MAX_FRAME_SIZE
#define TEMPLATE_SIZE    BR_MAX_FRAME_SIZE*2 

struct RConfig {
    uint16_t    magik;
    /// General parameters
    bool        inverted;       ///< The reed is inverted
    bool        led;            ///< The led blinks during notification.
    bool        enable_int;     ///< Enable INT1 external wake up
    uint16_t    id;             ///< Arbitrary device id (16bit uinteger)
    uint16_t    report;         ///< Report multiplier (Report every n*8sec) if n=0 disabled
    uint8_t     accuracy;       ///< Accuracy byte for sensors.
    char        payload_template[TEMPLATE_SIZE+1]; ///< RF payload template
    rf_mode_t   rf_mode;        ///< RF mode (PLAIN, MESH, ....)
    
    // Radio Parameters
    RadioConfig rf;
    
};

#define BOROSREED_EEPROM_OFFSET 0
enum config_par_t {
    CGF_MAGIK=offsetof(RConfig,magik),     
    CFG_INVERTED=offsetof(RConfig,inverted),
    CFG_LED=offsetof(RConfig,led),
    CFG_ENABLE_INT=offsetof(RConfig,enable_int),
    CFG_ID=offsetof(RConfig,id),        
    CFG_REPORT=offsetof(RConfig,report),
    CFG_ACCURACY=offsetof(RConfig,accuracy),
    CFG_PAYLOAD_TEMPLATE=offsetof(RConfig,payload_template),
    CFG_RF_MODE=offsetof(RConfig,rf_mode),
    #if defined(BOROSREED24)
    CFG_RF_PA_LEVEL=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_PA_level),
    CFG_RF_CHANNEL=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_channel),
    CFG_RF_SPEED=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_speed),
    CFG_RF_ADDR_SIZE=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_addr_size),
    CFG_RF_DADDR=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_daddr),
    CFG_RF_PAYLOAD_SIZE=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_payload_size),
    CFG_RF_CRC=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_crc),
    CFG_RF_ACK=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_ack),
    CFG_RF_RETRY=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_retry),
    CFG_RF_RETRY_DELAY=offsetof(RConfig,rf) + offsetof(RadioConfig,rf_retry_delay),
    CFG_MSH_NODEID=offsetof(RConfig,rf) + offsetof(RadioConfig,msh_nodeid),
    CFG_MSH_FRAMEID=offsetof(RConfig,rf) + offsetof(RadioConfig,msh_frameid),
    CFG_MSH_DADDR=offsetof(RConfig,rf) + offsetof(RadioConfig,msh_daddr),
    CFG_MSH_FORCE_RENEW=offsetof(RConfig,rf) + offsetof(RadioConfig,msh_force_renew)

    #elif defined(BOROSREED_LORA)
    CFG_TTN_DEVEUI=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_devEUI),
    CFG_TTN_APPEUI=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_appEUI),
    CFG_TTN_APPKEY=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_appKey),
    CFG_TTN_PORT=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_port),
    CFG_TTN_CHANNEL=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_channel),
    CFG_TTN_SF=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_sf),
    CFG_TTN_JSF=offsetof(RConfig,rf) + offsetof(RadioConfig,ttn_jsf)
    #endif
};

void             init_config(bool reset=false);
//void save_adc_cal(float v);
void            dump_config(Print &p);
void            get_config(config_par_t e,void *dest,uint16_t len);
void            save_config(config_par_t e,void *src,uint16_t len);
inline void     set_cnf(config_par_t e,uint8_t v) { save_config(e,&v,sizeof(uint8_t)); }
inline void     set_cnfw(config_par_t e,uint16_t v) { save_config(e,&v,sizeof(uint16_t)); }
inline uint8_t  get_cnf(config_par_t e){ uint8_t c=0; get_config(e,&c,1); return c; }
inline uint16_t get_cnfw(config_par_t e){ uint16_t c=0; get_config(e,&c,2); return c; }


/**
 * @brief Global Status store relevant information that need reused fast.
 *        Some information is captured in RAM
 *  Pure status variables are:
 *      - reed: Value of the reed
 *      - battery: Volage of the bat in mV
 *      -
 * 
 *  Common attributres for all versions: 
 *    - inverted: If the reed is te be interpreted as inverted
 *    - led: If the led needs to be on during notification.
 *    - id: 16bit Id of the device
 *    - report: Report multiple.
 *    - rf_mode: mode of operation
 *    - payload_template: template to be rendered before sending payload.
 *    # Used only fro plain rf24
 *      - rf_ack: Use ack
 *    # Used only for rf24 mesh 
 *      - msh_frame_id: ID of the frame in the mesh network
 *      - msh_daddr:    ID of the destination node.
 * 
 *  
 * 
 * 
 * 
 */


struct R24Status {
    bool      inverted;         ///< Consider Reed inverted
    bool      led;              ///< Flash led on notification
    bool      enable_int;       ///< Enable external int
    uint16_t  id;               ///< Internal 16bit ID
    uint16_t  report;           ///< Report multiplier n*8sec
    rf_mode_t rf_mode;          ///< RF Mode
    char      payload_template[TEMPLATE_SIZE+1];    ///< Payload template
#if defined(BOROSREED24)
    bool      rf_ack;              ///< Use autoack (for pipes only)
    #if defined(BOROSREED24_MESH)
        uint8_t   msh_frameid;     ///< Mesh frame
        uint8_t   msh_daddr;       ///< Mesh remote addr
        bool      msh_force_renew;  ///< Force Mesh renew address allways after sleep
    #endif
#elif defined(BOROSREED_LORA)
    uint8_t   ttn_channel;  ///< Fix Channel
    uint8_t   ttn_sf;       ///< Fix SF
    uint8_t   ttn_jsf;      ///< SF used in join
    uint8_t   ttn_port;     ///< port to be used.
#endif
    bool      reed;         ///< Status of the reed
    uint16_t  batery;       ///< Bat voltage in mV
    uint16_t  vcc;          ///< Vcc voltage in mV
    int16_t   temp;         ///< Temperature in Celsius 0.1C steps
    uint16_t  hum;          ///< Humidity in % 0.01 % steps
    uint16_t  preasure;     ///< Preasure
    uint8_t   accuracy;     ///< Sendor accuracy byte

    uint8_t buffer[BUFFER_SIZE+1]; ///< Shared buffer

};

extern R24Status status;

#endif