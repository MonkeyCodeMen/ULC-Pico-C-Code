
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "Debug.hpp"
#include "helper.h"

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_ARCH_MBED_RP2040)|| defined(ARDUINO_ARCH_RP2040)
  #include "malloc.h"
#endif


volatile bool   Debug::_initDone = false;
HardwareSerial *Debug::_pOut = NULL;
Mutex           Debug::_mutex;

Debug debug;


Debug::Debug()
{
}

void Debug::begin(HardwareSerial * pOut,uint32_t baud){
  if (pOut == NULL){
    _initDone = false;
  }else{
    _pOut=pOut;
    _pOut->begin(baud);
    _mutex.free();
    _initDone = true;
  }
}


bool Debug::_check(){
    if (_initDone == false){ return false;    }
    return true;
}

void Debug::log(const char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out(F("LOG("));
    _out(time.c_str());
    _out(F(")::"));
    _out(text);
    _outEnd();
    _mutex.free();
}


void Debug::log(const char * file,int line,const char * text,int value){
    String str;
    str = text;
    str += String(value);
    Debug::log(file,line,str.c_str());
}


void Debug::log(const char * file,int line,const char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out(F("LOG("));
    _out((char *)time.c_str());
    _out(F("):"));
    _out(file);
    _out(F(":"));
    String lineStr(line);
    _out((char *)lineStr.c_str());
    _out(F("::"));
    _out(text);
    _outEnd();
    _mutex.free();
}



void Debug::assertTrue(bool cond ,const char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());

        _mutex.lock();
        _out(F("ASSERT failed("));
        _out(time);
        _out(F(")::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}

void Debug::assertTrue(bool cond ,const char * file,int line,const char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());
        
        _mutex.lock();
        _out(F("ASSERT failed("));
        _out(time);
        _out(F("):"));
        _out(file);
        _out(F(":"));
        String lineStr(line);
        _out(lineStr);
        _out(F("::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}

void Debug::_outEnd(){
    // at the moment only simply dump to terminal
    _pOut->println();
    _pOut->flush();
}

void Debug::logMem(const char * file,int line,const char * text){
    if (_check() == false) return;
    int usedHeap = 0; 
    int freeHeap = 0; 
     #if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP8266)

    #elif defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_ARCH_MBED_RP2040)|| defined(ARDUINO_ARCH_RP2040)
      usedHeap = rp2040.getUsedHeap();
      freeHeap = rp2040.getFreeHeap();
    #else
    
    #endif
    String temp(millis());

    _mutex.lock();
    
    _out(F("LOG memory("));
    _out(temp);
    _out(F("):"));
    _out(file);
    _out(F(":"));
    //temp = String(line);
    temp = line;
    _out(temp);
    _out(F("::"));
    _out(text);
    temp="-memory status:\n\tused heap: "+String(usedHeap)+"\n\tfree heap: "+String(freeHeap);
    _out(temp);
    
    _outEnd();
    _mutex.free();
}




void Debug::dump(const char * pName,void *pIn, uint8_t length){
  String out = debug.hexDump((uint8_t*)pIn,length);
  _pOut->print(pName);
  _pOut->print(F(" : "));
  _pOut->println(out);
}

void Debug::dump(const char * pName,uint32_t value){
  _pOut->print(pName);
  _pOut->print(F(" : "));
  _pOut->print(value);  
  _pOut->println();
}


void Debug::dump(const char * pName,uint32_t value, int base){
  _pOut->print(pName);
  _pOut->print(F(" : "));
  _pOut->print(value,base);  
  _pOut->println();
}

void Debug::dump(const char * pName,const char * value){
  _pOut->print(pName);
  _pOut->print(F(" : "));
  _pOut->print(value);  
  _pOut->println();
}


String Debug::hexDump(uint8_t  * p,uint8_t length,const char * sep,const char * prefix){
  const char * transTable = "0123456789ABCDEF";
  bool first = true;
  uint8_t value,index;

  String out = "";
  
  for(uint8_t i=0;i < length;i++){
    // interspace (not at first byte)
    if (first == true){
      first = false;
    } else {
      out+=sep;
    }

    // get byte and add prefix
    value = *p;
    out+=prefix;
    
    // bits 7..4
    index = (value >> 4) & 0x0F;
    out+=transTable[index];

    // bits 3..0
    index = value & 0x0F;
    out+=transTable[index];
    p++;
  }
  return out;
}

void Debug::stop(const char * file,int line,const char * message){
  _pOut->print(F("### critical error - system stop ### file: <"));
  _pOut->print(file);
  _pOut->print(F("> in line :"));
  _pOut->print(line);
  _pOut->print(F(" :: "));
  _pOut->print(message);
  while (1){ };
}