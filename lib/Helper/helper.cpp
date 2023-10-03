#include "helper.hpp"
#include "Debug.hpp"

u32_t convertStrToInt(const char * str){
    int len = strlen(str);

    if (len == 0){
        return 0;
    }


    if ((len >2) && (str[0] == '0') && ((str[1] == 'x') || (str[1]=='X'))){
        // hex conversion
        if (len>10){
            LOG("convertStrToInt: hex convert string to long");
            return 0;     
        }
        u32_t sum = 0;
        u32_t value;
        char  pos=2;
        while (pos < len){
            sum = sum<<4;
            switch(str[pos]){
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
                    LOG("convertStrToInt: hex convert invalid digit found");
                    return 0;  
            }
            sum+=value;
            pos++;
        }
        return sum;     // return sum for hex conversion
    }

    if ((str[0] == '-')){
        if (len == 1){
            LOG("convertStrToInt: negative numbers string to short");
            return 0;
        }
        LOG("convertStrToInt: negative numbers until now not implemented");
        return 0;
    }

    u32_t sum = 0;
    u32_t value;
    char  pos=0;
    while (pos < len){
        sum = sum*10;
        switch(str[pos]){
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
                LOG("convertStrToInt: dez convert invalid digit found");
                return 0;  
        }
        sum+=value;
        pos++;
    }
    return sum;     // return sum for dez convert
}


u32_t convertStrToInt(String str){
    return convertStrToInt(str.c_str());
}

u32_t clamp(u32_t lowEnd,u32_t value,u32_t highEnd){
    if (value < lowEnd)     return lowEnd;
    if (value > highEnd)    return highEnd;
    return value;
}

