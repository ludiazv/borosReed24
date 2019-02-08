/**
 * @file radio.hpp
 * @brief 
 * @version 0.1
 * @date 2019-01-30
 * 
 * @copyright Copyright (c) 2019
 * 
 * This file is only wrapper for radio hal abstraction 
 */
#ifndef _H_BOROSREED24_RADIO_H_
#define _H_BOROSREED24_RADIO_H_

#if defined(BOROSREED24)
#include "radio24.hpp"
#define radio_init()            ( radio24_init() )
#define radio_connect()         ( radio24_connect() )
#define radio_disconnect()      ( radio24_disconnect() )
#define radio_pwr(x)            ( radio24_pwr( (x) ) )
#define radio_send(x,y)         ( radio24_send( (x) , (y) ) )
#elif defined(BOROSREED_LORA)
#include "radio_lora.hpp"
#define radio_init()            ( radioL_init() )
#define radio_connect()         ( radioL_connect() )
#define radio_disconnect()      ( radioL_disconnect() )
#define radio_pwr(x)            ( radioL_pwr( (x) ) )
#define radio_send(x,y)         ( radioL_send( (x) , (y) ) )
#endif

#endif
