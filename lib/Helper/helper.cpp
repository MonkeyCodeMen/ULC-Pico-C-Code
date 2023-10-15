#include "helper.hpp"
#include "Debug.hpp"

//String emptyString(F(""));

u32_t convertHexStrToInt(const char * str,int len){
    int digitFound=0;  
    u32_t sum = 0;
    u32_t value;
    int i = 0;

    while (len > 0){
        if (str[i] == ' '){
            i++;
            len--;
        } else {
            sum = sum<<4;
            digitFound++;
            if (digitFound > 8){
                LOG(F("convertHexStrToInt: hex convert string to long"));
                return 0;     
            }
            switch(str[i]){
                case '0':   value=0;    break;
                case '1':   value=1;    break;
                case '2':   value=2;    break;
                case '3':   value=3;    break;
                case '4':   value=4;    break;
                case '5':   value=5;    break;
                case '6':   value=6;    break;
                case '7':   value=7;    break;
                case '8':   value=8;    break;
                case '9':   value=9;    break;
                case 'a':   
                case 'A':   value=10;   break;
                case 'b':   
                case 'B':   value=11;   break;
                case 'c':   
                case 'C':   value=12;   break;
                case 'd':   
                case 'D':   value=13;   break;
                case 'e':   
                case 'E':   value=14;   break;
                case 'f':   
                case 'F':   value=15;   break;
                default:    
                    LOG(F("convertHexStrToInt: hex convert invalid digit found"));
                    return 0;
            }  
            sum+=value;
            i++;
            len--;
        }
    }
    return sum;     // return sum for hex conversion
}

u32_t convertDecStrToInt(const char * str,int len){
    int digitFound=0;  
    u32_t sum = 0;
    u32_t value;
    char  i=0;
    int   factor = 1;

    if ((str[0] == '-')){
        if (len == 1){
            LOG(F("convertDecStrToInt: negative numbers string to short"));
            return 0;
        }
        factor = -1;
    }

    while (i < len){
        if (str[i] == ' '){
            i++;
            len--;
        } else {
            sum = sum*10;
            digitFound++;
            if (digitFound > 10){
                LOG(F("convertDecStrToInt: dec convert string to long"));
                return 0;     
            }
            switch(str[i]){
                case '0':   value=0;    break;
                case '1':   value=1;    break;
                case '2':   value=2;    break;
                case '3':   value=3;    break;
                case '4':   value=4;    break;
                case '5':   value=5;    break;
                case '6':   value=6;    break;
                case '7':   value=7;    break;
                case '8':   value=8;    break;
                case '9':   value=9;    break;
                default:    
                    LOG(F("convertStrToInt: dez convert invalid digit found"));
                    return 0;  
            }
            sum+=value;
            i++;
            len--;
        }
    }
    if (factor != 1){
        s32_t val = factor * sum;
        sum = (u32_t) val;
    }
    return sum;     // return sum for dez convert
}


u32_t convertStrToInt(const char * str){
    int len = strlen(str);
    int i = 0;
    
    if (len == 0){
        return 0;  // on empty string
    }

    // skip leading spaces
    while (str[i] == ' '){
        i++;
        len--;
    }

    if ((len >2) && (str[i] == '0') && ((str[i+1] == 'x') || (str[i+1]=='X'))){
        return convertHexStrToInt(&str[i+2],len-2);     // looks like HEX string
    } 
    
    return convertDecStrToInt(&str[i],len); // looks like DEC string
}


u32_t convertStrToInt(String str){
    return convertStrToInt(str.c_str());
}

u32_t clamp(u32_t lowEnd,u32_t value,u32_t highEnd){
    if (value < lowEnd)     return lowEnd;
    if (value > highEnd)    return highEnd;
    return value;
}


u16_t color565(u8_t r,u8_t g,u8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

u16_t color565(u32_t c)
{
  return ((((c>>16) & 0xFF) / 8) << 11) | ((((c>>8)& 0xFF) / 4) << 5) | ((c & 0xFF) / 8);
}



u32_t get888ColorWheel(u8_t pos){
    u32_t res;
    HHH_BYTE(res) = 0;

    pos = 255 - pos;
    if(pos < 85) {
        HH_BYTE(res) = 255 - pos * 3; // R
        H_BYTE(res)  = 0;             // G
        L_BYTE(res)  = pos*3;         // B
    } else if(pos < 170) {
        pos -= 85;
        HH_BYTE(res) = 0;             // R
        H_BYTE(res)  = pos * 3;       // G
        L_BYTE(res)  = 255 - pos * 3; // B
    } else {
        pos -= 170;
        HH_BYTE(res) = pos * 3;       // R
        H_BYTE(res)  = 255 - pos * 3; // G 
        L_BYTE(res)  = 0;             // B
    }
    return res;
}
