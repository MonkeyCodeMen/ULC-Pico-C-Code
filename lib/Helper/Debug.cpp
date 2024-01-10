#include "Debug.hpp"
#include "helper.h"
#include "malloc.h"


volatile bool   Debug::_initDone = false;
Stream *        Debug::_pOut = NULL;
Mutex           Debug::_mutex;

Debug debug;


Debug::Debug()
{
}

void Debug::begin(Stream * pOut){
  if (_pOut == NULL){
    _initDone = false;
  }else{
    _pOut=pOut;
    _mutex.free();
    _initDone = true;
  }
}

bool Debug::_check(){
    if (_initDone == false){ return false;    }
    return true;
}

void Debug::log(char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out((char*)F_CHAR("LOG("));
    _out((char *)time.c_str());
    _out((char*)F_CHAR(")::"));
    _out(text);
    _outEnd();
    _mutex.free();
}

void Debug::log(char * file,int line,char * text,int value){
    String str;
    str = text;
    str += String(value);
    Debug::log(file,line,(char *)str.c_str());
}


void Debug::log(char * file,int line,char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out((char*)F_CHAR("LOG("));
    _out((char *)time.c_str());
    _out((char*)F_CHAR("):"));
    _out(file);
    _out((char*)F_CHAR(":"));
    String lineStr(line);
    _out((char *)lineStr.c_str());
    _out((char*)F_CHAR("::"));
    _out(text);
    _outEnd();
    _mutex.free();
}

void Debug::assertTrue(bool cond ,char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());

        _mutex.lock();
        _out((char*)F_CHAR("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F_CHAR(")::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}

void Debug::assertTrue(bool cond ,char * file,int line,char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());
        
        _mutex.lock();
        _out((char*)F_CHAR("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F_CHAR("):"));
        _out(file);
        _out((char*)F_CHAR(":"));
        String lineStr(line);
        _out((char *)lineStr.c_str());
        _out((char*)F_CHAR("::"));
        _out(text);
        _outEnd();
        _mutex.free();
    }
}


void Debug::_out(char * text){
    // at the moment only simply dump to terminal
    _pOut->print(text);
}

void Debug::_outEnd(){
    // at the moment only simply dump to terminal
    _pOut->println();
    _pOut->flush();
}

void Debug::logMem(char * file,int line,char * text){
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
    
    _out((char*)F_CHAR("LOG memory("));
    _out((char *)temp.c_str());
    _out((char*)F_CHAR("):"));
    _out(file);
    _out((char*)F_CHAR(":"));
    //temp = String(line);
    temp = line;
    _out((char *)temp.c_str());
    _out((char*)F_CHAR("::"));
    _out(text);
    temp="-memory status:\n\tused heap: "+String(usedHeap)+"\n\tfree heap: "+String(freeHeap);
    _out((char *)temp.c_str());
    
    _outEnd();
    _mutex.free();
}




void Debug::dump(const char * pName,void *pIn, uint8_t length){
  uint8_t * p=(uint8_t*)pIn;
  _pOut->print(pName);
  _pOut->print(" : ");
  for(uint8_t i=0;i < length;i++) {
    _pOut->print(p[i],HEX);  
    _pOut->print('.');
  }
  _pOut->println();
}

void Debug::dump(const char * pName,uint32_t value){
  _pOut->print(pName);
  _pOut->print(" : ");
  _pOut->print(value);  
  _pOut->println();
}


void Debug::dump(const char * pName,uint32_t value, int base){
  _pOut->print(pName);
  _pOut->print(" : ");
  _pOut->print(value,base);  
  _pOut->println();
}

void Debug::dump(const char * pName,String value){
  _pOut->print(pName);
  _pOut->print(" : ");
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

void Debug::stop(const char * file,int line,char * message){
  _pOut->print("### critical error - system stop ### file: <");
  _pOut->print(file);
  _pOut->print("> in line :");
  _pOut->print(line);
  _pOut->print(" :: ");
  _pOut->print(message);
  while (1){ };
}