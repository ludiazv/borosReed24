#ifdef BOROSREED24
#include "radio24.hpp"
#include <SPI.h>
#include <RF24.h>

// Special includes for Mesh
#if defined(BOROSREED24_MESH)
#include <RF24Network.h>
#include <RF24Mesh.h>
#endif

// Configuration pins Arduino IOS
#define CS_PIN          10
#define CE_PIN          9 
#define MESH_TIMEOUT    30000U      //30sec

// Radio objects
static RF24 radio(CE_PIN,CS_PIN);
#if defined(BOROSREED24_MESH)
static RF24Network network(radio);
static RF24Mesh mesh(radio,network);
#endif

/**
 * @brief Powers de
 * 
 * @param v true power up / false power down 
 */
void radio24_pwr(bool v){
    if(v) radio.powerUp(); else radio.powerDown(); 
}

/**
 * @brief Init the radio
 * 
 * @return true 
 * @return false 
 */
bool radio24_init() {
    bool ret=false;
    if(status.rf_mode==RF_PLAIN) {
        ret=radio.begin() && radio.isChipConnected() &&
            radio.setDataRate((rf24_datarate_e)get_cnf(CFG_RF_SPEED));
        if(ret) {
            
            radio.maskIRQ(0,0,0); // No mask any IRQ
            radio.setAddressWidth(5); // Reset Address w.
            radio.setPayloadSize(32); // Reset Payload size
            uint8_t tmp=get_cnf(CFG_RF_CRC);
            if(tmp) radio.setCRCLength((rf24_crclength_e)tmp); else radio.disableCRC(); // CRC
            radio.setPALevel(get_cnf(CFG_RF_PA_LEVEL),get_cnf(CFG_RF_ENABLE_LNA)); // Cofigure PA
            radio.setChannel(get_cnf(CFG_RF_CHANNEL)); // Configure Channel
            tmp=get_cnf(CFG_RF_PAYLOAD_SIZE);
            if(tmp==0) radio.enableDynamicPayloads();   // Activate DynamicPayloads 
            else radio.setPayloadSize(max(tmp,32));     // Selected payload size
            tmp=get_cnf(CFG_RF_ADDR_SIZE);
            radio.setAddressWidth(tmp);
            uint8_t pipe[5]={0,0,0,0,0};
            get_config(CFG_RF_DADDR,pipe,tmp);
            radio.setAutoAck(get_cnf(CFG_RF_ACK));
            radio.setRetries(get_cnf(CFG_RF_RETRY_DELAY),get_cnf(CFG_RF_RETRY));
            radio.openWritingPipe(pipe);

            radio.txDelay=0; // No delay
            radio.startListening();
            radio.stopListening();
            radio.flush_tx();

        }

    }
    #if defined(BOROSREED24_MESH) 
    else if(status.rf_mode==RF_MESH) {
        uint8_t id=get_cnf(CFG_MSH_NODEID);
        if(id==0) return ret;
        mesh.setNodeID(id);
        ret= mesh.begin(get_cnf(CFG_RF_CHANNEL),(rf24_datarate_e)get_cnf(CFG_RF_SPEED),MESH_TIMEOUT);
        if(ret) {
            mesh.setChild(false);
            radio.setPALevel(get_cnf(CFG_RF_PA_LEVEL),get_cnf(CFG_RF_ENABLE_LNA)); // Cofigure PA
        }
    }
    #endif
    radio.powerDown(); // Radio is disabled to save energy
    return ret;
}

bool radio24_connect() {
    if(status.rf_mode==RF_PLAIN) {
        radio.flush_tx();
        return true;
    } 
    #if defined(BOROSREED24_MESH)
    else if(status.rf_mode==RF_MESH) {
        if(status.msh_force_renew) {
            return mesh.renewAddress(MESH_TIMEOUT);
        } else {
            if(mesh.checkConnection()) return true;
            return mesh.renewAddress(MESH_TIMEOUT);
        }
    } 
    #endif
    else return false;
}
void radio24_disconnect() {
    if(status.rf_mode==RF_PLAIN) { radio.flush_tx(); return;}
    #if defined(BOROSREED24_MESH)
    if(status.rf_mode==RF_MESH && status.msh_force_renew) mesh.releaseAddress();
    #endif
}

bool radio24_send(const void *b,uint8_t size) {
    if(status.rf_mode==RF_PLAIN) {
        bool irq_rx,irq_tx,irq_maxrt;
        radio.startWrite(b,size,!status.msh_force_renew);
        wake_t w=radioSleep();
        radio.whatHappened(irq_tx,irq_maxrt,irq_rx); // Clean IRQ & get info
        if(w==WAKE_RADIO) {
            return irq_tx;
        } else return false;
    }
    #if defined(BOROSREED24_MESH) 
    else if(status.rf_mode==RF_MESH) {
       return mesh.write(b,status.msh_frameid,size,status.msh_daddr);
    } 
    #endif
    else return false;
}

#endif


