#ifndef _H_BOROSREED24_RADIOLORA_H_
#define _H_BOROSREED24_RADIOLORA_H_

#include "status.hpp"
#include "hal.hpp"
#include "dbg.h"

// Prototypes
bool radioL_init();
bool radioL_connect();
void radioL_disconnect();
void radioL_pwr(bool on=false);
bool radioL_send(const void* payload,uint8_t size);


#endif