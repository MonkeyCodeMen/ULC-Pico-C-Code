#include <helper.h>
//extern String emptyString;

uint32_t convertHexStrToInt(const char * str,int len){
    int   digitFound=0;  
    uint32_t sum = 0;
    uint8_t  value;
    int   i = 0;

    while (i < len){
        if (str[i] == ' '){
            i++;
        } else {
            sum = sum<<4;
            digitFound++;
            if (digitFound > 8){
                //LOG(F("convertHexStrToInt: hex convert string to long"));
                sum = 0;
                return sum;     
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
                    //LOG(F("convertHexStrToInt: hex convert invalid digit found"));
                    sum = 0;
                    return sum;
            }  
            sum+=value;
            i++;
        }
    }
    return sum;     // return sum for hex conversion
}

uint32_t convertDecStrToInt(const char * str,int len){
    int   digitFound=0;  
    uint32_t sum = 0;
    uint8_t  value;
    int   i=0;
    int   factor = 1;

    if ((str[0] == '-')){
        if (len == 1){
            //LOG(F("convertDecStrToInt: negative numbers string to short"));
            sum = 0;
            return sum;
        }
        i++;
        factor = -1;
    }

    while (i < len){
        if (str[i] == ' '){
            i++;
        } else {
            sum = sum*10;
            digitFound++;
            if (digitFound > 10){
                //LOG(F("convertDecStrToInt: dec convert string to long"));
                sum=0;
                return sum;     
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
                    //LOG(F("convertStrToInt: dez convert invalid digit found"));
                    sum = 0;
                    return sum;  // reuse sum for compiler optimization = avoid result copy  
            }
            sum+=value;
            i++;
        }
    }
    if (factor != 1){
        int32_t val = factor * sum;
        sum = (uint32_t) val;
    }
    return sum;     // return sum for dez convert
}


uint32_t convertStrToInt(const char * str){
    int   len = strlen(str);
    int   i = 0;
    uint32_t res = 0;
    
    if (len == 0){
        return res;  // on empty string
    }

    // skip leading spaces
    while (str[i] == ' '){
        i++;
        len--;
        if (len == 0){
            return res;
        }
    }

    if ((len >2) && (str[i] == '0') && ((str[i+1] == 'x') || (str[i+1]=='X'))){
        res = convertHexStrToInt(&str[i+2],len-2);      // looks like HEX string
    } else {
        res = convertDecStrToInt(&str[i],len);          // looks like DEC string;
    }
    
    return res; 
}


uint32_t convertStrToInt(String str){
    return convertStrToInt(str.c_str());
}

uint32_t clamp(uint32_t lowEnd,uint32_t value,uint32_t highEnd){
    if (value < lowEnd)     return lowEnd;
    if (value > highEnd)    return highEnd;
    return value;
}


uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b,uint8_t dim){
    return toColor565( dimColorChannel255(r,dim),dimColorChannel255(g,dim),dimColorChannel255(b,dim) );
}

uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b){
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t toColor565(uint32_t c){
  return ((((c>>16) & 0xFF) / 8) << 11) | ((((c>>8)& 0xFF) / 4) << 5) | ((c & 0xFF) / 8);
}

uint16_t toColor565Dim(uint32_t c){
    c = dimRgb24ToRgb(c);
    return toColor565(c);
}

uint16_t getColorWheel565(uint8_t pos){
    uint32_t res = getColorWheel24Bit(pos);
    return toColor565(res);
}


uint32_t getColorWheel24Bit(uint8_t pos){
    uint32_t res;
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

uint32_t dimRgb24ToRgb(uint32_t color){
    union uint32_t_byteAcess value;
    value.ival=color;
    uint8_t dim = value.bval.HHH;
    value.bval.HH = dimColorChannel255(value.bval.HH,dim); // r
    value.bval.H  = dimColorChannel255(value.bval.H ,dim); // g
    value.bval.L  = dimColorChannel255(value.bval.L ,dim); // b
    value.bval.HHH = 0;
    return value.ival;
}

uint32_t dimColor255(uint32_t color,uint8_t dim ){
    union uint32_t_byteAcess value;
    value.ival=color;
    value.bval.HH = dimColorChannel255(value.bval.HH,dim); // r
    value.bval.H  = dimColorChannel255(value.bval.H ,dim); // g
    value.bval.L  = dimColorChannel255(value.bval.L ,dim); // b
    value.bval.HHH = 0;
    return value.ival;
}

uint32_t dimColor255(uint8_t r,uint8_t g,uint8_t b,uint8_t dim ){
    union uint32_t_byteAcess value;
    value.bval.HH = dimColorChannel255(r,dim); // r
    value.bval.H  = dimColorChannel255(g,dim); // g
    value.bval.L  = dimColorChannel255(b,dim); // b
    value.bval.HHH = 0;
    return value.ival;

}

uint32_t color24Bit(uint8_t r,uint8_t g,uint8_t b){
    union uint32_t_byteAcess value;
    value.bval.HH   = r; // r
    value.bval.H    = g; // g
    value.bval.L    = b; // b
    value.bval.HHH  = 0;
    return value.ival;

}


