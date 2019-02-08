
#ifndef BOROSREED24_VERSION
#define _HC_VERSION_BR_ "V0.0.1"

#if defined(BOROSREED_LORA)
#define BOROSREED24_VERSION _HC_VERSION_BR_ "[LORA]"
#elif defined(BOROSREED24) 
    #if defined(BOROREED24_MESH)
        #define BOROSREED24_VERSION _HC_VERSION_BR_ "[24M]"
    #else
        #define BOROSREED24_VERSION _HC_VERSION_BR_ "[24]"   
    #endif
#endif

#endif