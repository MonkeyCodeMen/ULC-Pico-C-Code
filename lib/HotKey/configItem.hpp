#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <helper.h>
#include <Ani.hpp>
#include <LedObjects.hpp>





/****
 * 
 * JSON config file for configItem
 * 
 * {
 *      "obj"  : "LED0",
 *      "cmd"  : "off",
 *      "param": ["0x00","","",""],
 *      "str"  : ""
 * }
 * 
*/



class configItem{
    public:
        configItem():_obj(NULL),_cmd(""),_cfg()                                         { }
        configItem(const configItem & src):_obj(src._obj),_cmd(src._cmd),_cfg(src._cfg) { } 
        configItem(JsonDocument config){
                _obj = objNameToPtr((const char*) config["obj"]);
                _cmd = (const char *) config["cmd"]  ;
                _cfg.str = (const char *) config["str"]  ;
                
                // fix NULL pointer return
                if (_cmd.c_str() == NULL)       _cmd = "";  
                if (_cfg.str.c_str() == NULL)   _cfg.str = "";

                _cfg.dimCfg.uint32    = (uint32_t) convertStrToInt( (const char *) config["param"][0] );
                _cfg.colorCfg.uint32  = (uint32_t) convertStrToInt( (const char *) config["param"][1] );
                _cfg.flashCfg.uint32  = (uint32_t) convertStrToInt( (const char *) config["param"][2] );
                _cfg.breathCfg.uint32 = (uint32_t) convertStrToInt( (const char *) config["param"][3] );
            }
        ~configItem()                           { clear();                                                  }

        CtrlPtr obj()                           { return _obj;                                              }   
        String cmd()                            { return _cmd;                                              }
        AniCfg cfg()                            { return _cfg;                                              }
        void clear()                            { _obj = NULL;      _cmd = "";          _cfg = AniCfg();    }
        void operator=(const configItem & src)  { _obj = src._obj;  _cmd = src._cmd;    _cfg = src._cfg;    }
        bool run(){
                if (_obj != NULL){
                    if (_cmd != "") {
                        if(_obj->select(_cmd.c_str()) != ANI_OK){
                            return false;
                        } 
                    }
                    if (_obj->config(_cfg) != ANI_OK){
                        return false;
                    }
                }
                return false;
            }
 
    protected:
        CtrlPtr     _obj;
        String      _cmd;
        AniCfg      _cfg;
};



