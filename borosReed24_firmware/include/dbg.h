#ifndef _H_BOROSREED24_DBG_H_
#define _H_BOROSREED24_DBG_H_

#define FH(x)   ((__FlashStringHelper*)(x)) 

#if defined(BOROSREED24_DBG)

#define DBG_SPEED       9600
#define DBG_INIT()      Serial.begin(DBG_SPEED)
#define DBG_PRINT(x)    Serial.print((x))
#define DBG_PRINTLN(x)  Serial.println((x))
#define DBG_PLN()       Serial.println();
#define DBG_FLUSH()     Serial.flush()
#define DBG_EXP(x)      (x)

#else

#define DBG_SPEED       9600    
#define DBG_INIT()      
#define DBG_PRINT(x)    
#define DBG_PRINTLN(x)
#define DBG_PLN()  
#define DBG_FLUSH()
#define DBG_EXP(x)


#endif

#endif