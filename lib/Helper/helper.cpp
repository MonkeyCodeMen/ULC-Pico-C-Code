#include <helper.h>
//extern String emptyString;

uint8_t convertCharToInt(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0'; // Convert numeric characters directly
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10; // Convert uppercase hex letters
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10; // Convert lowercase hex letters
    }
    return 255; // Default fallback for invalid characters
}

uint8_t convertDezCharToInt(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0'; // Convert numeric characters directly
    } 
    return 255; // Default fallback for invalid characters
}

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
            value = convertCharToInt(str[i]);
            if (value == 255){
                sum=0;
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
            value = convertDezCharToInt(str[i]);
            if (value == 255){
                sum=0;
                return sum;
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
    if (str == NULL)    return 0;
    int   len = strlen(str);
    if (len == 0)       return 0;

    int   i = 0;
    uint32_t res = 0;


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

uint32_t clampUint32(uint32_t lowEnd,uint32_t value,uint32_t highEnd){
    if (value < lowEnd)     return lowEnd;
    if (value > highEnd)    return highEnd;
    return value;
}

int32_t clamp(int32_t lowEnd,int32_t value,int32_t highEnd){
    if (value < lowEnd)     return lowEnd;
    if (value > highEnd)    return highEnd;
    return value;
}


int32_t wrapAround(int32_t lowEnd,int32_t value,int32_t highEnd){
    if ((value <= highEnd) && (value >= lowEnd))  return value;
    int32_t delta = highEnd-lowEnd+1;
    if (value > highEnd){
        return  ((value-lowEnd) % delta) + lowEnd;
    }
    
    while (value < lowEnd){
        value+=delta;
    }
    return value;
}


/*
uint32_t wrapAround(uint32_t lowEnd,uint32_t value,uint32_t highEnd){
    if ((value <= highEnd) && (value >= lowEnd))  return value;
    int32_t delta = highEnd-lowEnd+1;
    if (value > highEnd){
        return  ((value-lowEnd) % delta) + lowEnd;
    }
    while (value < lowEnd){
        value+=delta;
    }
    return value;

}



  const int mod = maxval + 1 - minval;
  if (delta >= 0) {return  (v + delta                - minval) % mod + minval;}
  else            {return ((v + delta) - delta * mod - minval) % mod + minval;}
*/


uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b,uint8_t dim){
    return toColor565( dimColorChannel255(r,dim),dimColorChannel255(g,dim),dimColorChannel255(b,dim) );
}

uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b){
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t toColor565(uint32_t c){
  return ((((c>>16) & 0xFF) / 8) << 11) | ((((c>>8)& 0xFF) / 4) << 5) | ((c & 0xFF) / 8);
}

uint16_t toColor565(uint32_t c,uint8_t dim){
  c = dimColor255(c,dim);
  return toColor565(c);
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
        HH_BYTE(res) = 255 - (pos * 3); // R
        H_BYTE(res)  = 0;               // G
        L_BYTE(res)  = pos*3;           // B
    } else if(pos < 170) {
        pos -= 85;
        HH_BYTE(res) = 0;               // R
        H_BYTE(res)  = pos * 3;         // G
        L_BYTE(res)  = 255 - (pos * 3); // B
    } else {
        pos -= 170;
        HH_BYTE(res) = pos * 3;         // R
        H_BYTE(res)  = 255 - (pos * 3); // G 
        L_BYTE(res)  = 0;               // B
    }
    return res;
}

uint32_t dimRgb24ToRgb(uint32_t color){
    union uint32_t_byteAcess value;
    value.uint32=color;
    uint8_t dim = value.byte.HHH;
    value.byte.HH = dimColorChannel255(value.byte.HH,dim); // r
    value.byte.H  = dimColorChannel255(value.byte.H ,dim); // g
    value.byte.L  = dimColorChannel255(value.byte.L ,dim); // b
    value.byte.HHH = 0;
    return value.uint32;
}

uint32_t dimColor255(uint32_t color,uint8_t dim ){
    union uint32_t_byteAcess value;
    value.uint32=color;
    value.byte.HH = dimColorChannel255(value.byte.HH,dim); // r
    value.byte.H  = dimColorChannel255(value.byte.H ,dim); // g
    value.byte.L  = dimColorChannel255(value.byte.L ,dim); // b
    value.byte.HHH = 0;
    return value.uint32;
}

uint32_t dimColor255(uint8_t r,uint8_t g,uint8_t b,uint8_t dim ){
    union uint32_t_byteAcess value;
    value.byte.HH = dimColorChannel255(r,dim); // r
    value.byte.H  = dimColorChannel255(g,dim); // g
    value.byte.L  = dimColorChannel255(b,dim); // b
    value.byte.HHH = 0;
    return value.uint32;

}

uint32_t color24Bit(uint8_t r,uint8_t g,uint8_t b){
    union uint32_t_byteAcess value;
    value.byte.HH   = r; // r
    value.byte.H    = g; // g
    value.byte.L    = b; // b
    value.byte.HHH  = 0;
    return value.uint32;

}


int findStringInArray(String * pFirst,int size,String target){
    int i=0;
    while (i<size){
        if (*pFirst == target){
            //string found
            return i;
        }
        i++;
        pFirst++;
    }
    // end reached ==> string not found 
    return -1;
}


String removeLeadingCharacters(String input, char charToRemove) {
    int startIndex = 0;
    
    // find first not matching position
    while (startIndex < input.length() && input[startIndex] == charToRemove) {
        startIndex++;
    }
    
    // return resulting substring
    return input.substring(startIndex);
}

String removeLeadingCharacters(String input, const String& charsToRemove) {
    int startIndex = 0;

    // find first not matching position
    while (startIndex < input.length() && charsToRemove.indexOf(input[startIndex]) != -1) {
        startIndex++;
    }

    // return resulting substring
    return input.substring(startIndex);
}


String removeTrailingCharacters(String input, char charToRemove) {
    int endIndex = input.length() - 1;
    
    // find last not matching position from tail
    while (endIndex >= 0 && input[endIndex] == charToRemove) {
        endIndex--;
    }
    
    // return resulting substring
    return input.substring(0, endIndex + 1);
}

String removeTrailingCharacters(String input, const String& charsToRemove) {
    int endIndex = input.length() - 1;

    // find last not matching position from tail
    while (endIndex >= 0 && charsToRemove.indexOf(input[endIndex]) != -1) {
        endIndex--;
    }

    // return resulting substring
    return input.substring(0, endIndex + 1);
}

