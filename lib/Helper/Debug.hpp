#pragma once

#include <Arduino.h>
#include <Mutex.hpp>


class Debug
{
public:
	Debug();
	~Debug() = default;

    void begin(HardwareSerial * pOut, uint32_t baud=115200);

    static void log(const char * text);
    static void log(const __FlashStringHelper * text)                                   { log(String(text));                                }
    static void log(char * text)                                                        { log((const char *)text);                          }
    static void log(String text)                                                        { log(text.c_str());                                }

    static void log(const char * file,int line,const char * text);
    static void log(const char * file,int line,const __FlashStringHelper * text)        { log(file,line,String(text));                      }
    static void log(const char * file,int line,char * text)                             { log(file,line,(const char *)text);                }
    static void log(const char * file,int line,String text)                             { log(file,line,text.c_str());                      }
    
    static void log(const char * file,int line,const char * text,int value);
    static void log(const char * file,int line,const __FlashStringHelper * text,int value){ log(file,line,String(text),value);              }
    static void log(const char * file,int line,char * text,int value)                   { log(file,line,(const char *)text,value);          }
    static void log(const char * file,int line,String text,int value)                   { log(file,line,text.c_str(),value);                }

    static void logMem(const char * file,int line,const char * text);
    static void logMem(const char * file,int line,const __FlashStringHelper * text)     { logMem(file,line,String(text));                   }
    static void logMem(const char * file,int line,char * text)                          { logMem(file,line,(const char*)text);              }
    static void logMem(const char * file,int line,String text)                          { logMem(file,line,text.c_str());                   }
    
    static void assertTrue(bool cond ,const char * text);
    static void assertTrue(bool cond ,const __FlashStringHelper * text)                 { assertTrue(cond,String(text));                    }
    static void assertTrue(bool cond ,char * text)                                      { assertTrue(cond,(const char*)text);               }
    static void assertTrue(bool cond ,String text)                                      { assertTrue(cond,text.c_str());                    }
    
    static void assertTrue(bool cond ,const char * file,int line,const char * text);
    static void assertTrue(bool cond ,const char * file,int line,const __FlashStringHelper * text){ assertTrue(cond,file,line,String(text));}
    static void assertTrue(bool cond ,const char * file,int line,char * text)           { assertTrue(cond,file,line,(const char *)text);    }
    static void assertTrue(bool cond ,const char * file,int line,String text)           { assertTrue(cond,file,line,text.c_str());          }

    static void stop(const char * file,int line,const char * message);
    static void stop(const char * file,int line,const __FlashStringHelper * message)    { stop(file,line,String(message));                  }
    static void stop(const char * file,int line,char * message)                         { stop(file,line,(const char*) message);            }
    static void stop(const char * file,int line,String message)                         { stop(file,line,message.c_str());                  }

    static void dump(const char * pName,void *p, uint8_t length);
    static void dump(char * pName,void *p,uint8_t length)                               { dump((const char *)pName,p,length);               }
    static void dump(const __FlashStringHelper * pName,void *p,uint8_t length)          { dump(String(pName),p,length);                     }
    static void dump(String name,void *p,uint8_t length)                                { dump(name.c_str(),p,length);                      }
    
    static void dump(const char * pName,uint32_t value);
    static void dump(char * pName,uint32_t value)                                       { dump((const char *)pName,value);                  }
    static void dump(const __FlashStringHelper * pName,uint32_t value)                  { dump(String(pName),value);                        }
    static void dump(String name,uint32_t value)                                        { dump(name.c_str(),value);                         }

    static void dump(const char * pName,uint32_t value,int base);
    static void dump(char * pName,void *p,uint32_t value,int base)                      { dump((const char *)pName,p,value,base);           }
    static void dump(const __FlashStringHelper * pName,void *p,uint32_t value,int base) { dump(String(pName),p,value,base);                 }
    static void dump(String name,void *p,uint32_t value,int base)                       { dump(name.c_str(),p,value,base);                  }


    static void dump(const char * pName,const char * value);
    static void dump(char * pName,const char * value)                                   { dump((const char *)pName,value);                  }
    static void dump(const __FlashStringHelper * pName,const char * value)              { dump(String(pName),value);                        }
    static void dump(String name,const char * value)                                    { dump(name.c_str(),value);                         }
    static void dump(char * pName,char * value)                                         { dump((const char *)pName,(const char *)value);    }
    static void dump(const __FlashStringHelper * pName,char * value)                    { dump(String(pName),(const char *)value);          }
    static void dump(String name,char * value)                                          { dump(name.c_str(),(const char *)value);           }
    static void dump(char * pName,String value)                                         { dump((const char *)pName,value.c_str());          }
    static void dump(const __FlashStringHelper * pName,String value)                    { dump(String(pName),value.c_str());                }
    static void dump(String name,String value)                                          { dump(name.c_str(),value.c_str());                 }


    String hexDump(uint8_t * p,uint8_t length,const char * sep=" ", const char * prefix="");


private:
    static void _out(char * text)                                                       {_pOut->print(text);}
    static void _out(const char * text)                                                 {_pOut->print(text);}
    static void _out(String  text)                                                      {_pOut->print(text);}
    static void _out(const __FlashStringHelper * text)                                  {_pOut->print(text);}
    static void _outEnd();
    static bool _check();

    volatile static bool    _initDone;
    static HardwareSerial * _pOut;
    static Mutex            _mutex;

};


extern Debug debug;

#define WITH_DEBUG 1
#ifdef WITH_DEBUG
    #define LOG(text)           Debug::log((const char*)__FILE__,__LINE__,text)
    #define LOG_INT(text,value) Debug::log((const char*)__FILE__,__LINE__,text,(int)value)
    #define LOG_MEM(text)       Debug::logMem((const char*)__FILE__,__LINE__,text)
    #define ASSERT(cond,text)   Debug::assertTrue(cond,(const char*)__FILE__,__LINE__,text)
    #define STOP(text)          Debug::stop((const char*)__FILE__,__LINE__,text)
    #define DUMP(...)           Debug::dump( __VA_ARGS__)
#else
    #define LOG(text)
    #define LOG_MEM(text)
    #define ASSERT(cond,text)   
    #define STOP()              Debug::stop((const char*)__FILE__,__LINE__,text)
    #define DUMP(...)
#endif

