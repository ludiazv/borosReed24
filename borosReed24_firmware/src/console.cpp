#include "console.hpp"

namespace Boros {

bool    Console::parse_bool(const ConsoleCmd *cmd) {
    return (cmd->param!=NULL && cmd->param[0]!='0');
}

/*uint16_t Console::parse_uint16(const ConsoleCmd *cmd) {
    if(cmd->param!=NULL)
        return atoi(cmd->param);
    else return 0;
}*/

uint32_t Console::parse_uint32(const ConsoleCmd *cmd) {
    if(cmd->param!=NULL) return atol(cmd->param); else return 0;
}

uint8_t Console::hex(char ca) {
    char c=toupper(ca);
    //Serial.print("C:");  Serial.println(c,HEX);
    //Serial.print("CA:"); Serial.println(ca,HEX);
    if(c>='0' && c<='9'){
        return c - '0';
    } else if(c>='A' && c<='F') {
        return 0x0A + (c - 'A');
    } else return 0xFF;
}

bool Console::parse_hex(const ConsoleCmd *cmd,uint8_t *dst,uint8_t size) {
    if(cmd->param==NULL) return false;
    int l=strlen(cmd->param);
    //Serial.print("L:");Serial.print(l);
    //Serial.print(" S:");Serial.println(size);
    //Serial.print("{"); Serial.print(cmd->param);
    //Serial.print("}");
    if(l==0 || (l % 2) > 0 || l != (size*2)) return false;
    for(uint8_t i=0;i<l;i+=2) {
        char ch,ch2;
        ch=this->hex(cmd->param[i]);
        ch2=this->hex(cmd->param[i+1]);
        //Serial.print("CH1:"); Serial.println(ch,HEX);
        //Serial.print("CH2:"); Serial.println(ch2,HEX);
        if(ch!=0xFF && ch2!=0xFF) {
            *dst=(ch << 4) | ch2;
            //Serial.print(*dst,HEX);
            dst++;
        } else {
            return false;
        }
    }
    return true;
}

uint8_t Console::loop(ConsoleCmd *cmd){

    if(stat==IN_PROMPT) {
        //BOROS_CONSOLE.println();
        BOROS_CONSOLE.print(prompt);
        stat= IN_CMD;
        cmd->cmd=0;
        bpos=0;
    }
    
    if(!BOROS_CONSOLE.available()) return 0; // if no avaialble
    char c=BOROS_CONSOLE.read(); // Free input buffer
    if(bpos>=lbuffer) {
        BOROS_CONSOLE.print("\nEntry too long");
        stat=IN_PROMPT;
        return 0;
    }
    bool sep=(c=='\r' || c=='\n');// CR, LF , 
    bool spc=(c==' ');  // SPACE as command separator
    if(sep) {
        delay(150);
        if(BOROS_CONSOLE.available()){
                char c2=BOROS_CONSOLE.peek();
                if(c2=='\r' || c2=='\n') BOROS_CONSOLE.read();
        }
    }
    
    if(stat == IN_CMD) {
        if(sep || c==' '){ 
            buffer[bpos]='\0'; 
            if((c=check_command())>0) {
                if(spc) BOROS_CONSOLE.print(' '); else BOROS_CONSOLE.println();
                cmd->cmd=c;
                stat=IN_PAR;
                bpos=0;
                return 1;
            } else {
                BOROS_CONSOLE.println(F("\nInvalid command"));
                stat=IN_PROMPT;
            }
        } else {
            buffer[bpos++]=c;
            BOROS_CONSOLE.print(c);
        }
        //BOROS_CONSOLE.print(c);

    } else if(stat == IN_PAR) {
        if(sep) {
            buffer[bpos]='\0';
            cmd->param=buffer;
            stat=IN_PROMPT;
            return 2;
        } else {
            buffer[bpos++]=c;
            BOROS_CONSOLE.print(c);
        }
    }

    return 0;
}

uint8_t Console::check_command() {
    uint8_t i;
    for(i=0;i<n_commands;i++) {
        if(strcmp_P(buffer,commands[i])==0) break;
    }
    i=(i>=n_commands) ? 0: i+1;
    return i;
}


}
