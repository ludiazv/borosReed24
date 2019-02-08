#ifndef _H_BOROSCONSOLE_H_
#define _H_BOROSCONSOLE_H_

#include <Arduino.h>
#define  BOROS_CONSOLE Serial


namespace Boros {

 typedef char command_t[5+1];

 struct ConsoleCmd {
     uint8_t cmd;
     char *param; 
 };

 enum console_status_t {
    IN_PROMPT,
    IN_CMD,
    IN_PAR,
 };

 class Console {
     public:
        Console(const command_t* com,uint8_t n,
                char *buff,uint8_t l,
                char pro='>') : 
           commands(com), n_commands(n),
           buffer(buff), lbuffer(l), 
           prompt(pro), stat(IN_PROMPT), bpos(0) {};
 
        uint8_t  loop(ConsoleCmd *cmd);
        bool     parse_bool(const ConsoleCmd *com);
        uint32_t parse_uint32(const ConsoleCmd *com);
        inline uint16_t parse_uint16(const ConsoleCmd *com) { return parse_uint32(com); }
        inline uint8_t  parse_uint8(const ConsoleCmd *com)  { return parse_uint16(com); }
        bool     parse_hex(const ConsoleCmd *com,uint8_t *dst,uint8_t size);

        inline  void reset() { stat= IN_PROMPT; }
    private:
        uint8_t check_command();
        uint8_t hex(char c);
        const command_t*   commands;
        uint8_t n_commands;
        char*   buffer;
        uint8_t lbuffer;
        char    prompt;
        console_status_t stat;
        uint8_t bpos;
    
 };

}


#endif