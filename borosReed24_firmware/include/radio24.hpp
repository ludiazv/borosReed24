#ifndef _H_BOROSREED24_RADIO24_H_
#define _H_BOROSREED24_RADIO24_H_

// To use config information
#include "status.hpp"
#include "hal.hpp"

bool radio24_init();
bool radio24_connect();
void radio24_disconnect();
void radio24_pwr(bool on=false);
bool radio24_send(const void* payload,uint8_t size);


#endif