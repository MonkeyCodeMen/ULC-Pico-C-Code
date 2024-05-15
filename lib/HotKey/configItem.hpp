#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <helper.h>
#include <LedObjects.hpp>



/****
 * 
 * JSON config file for configItem
 * 
 * {
 *      "obj":    "LED0",
 *      "cmd":    "off",
 *      "param": ["0x00","","",""],
 *      "str":"",
 *      "bin":[0,2,255,40,30]
 * }
 * 
*/



class configItem{
    public:
        configItem():_pData(NULL),_dataSize(0),_obj(NULL),_cmd(""),_str(""),_param{0,0,0,0}
                                                {                       }
        configItem(const configItem & src ):_pData(NULL),_dataSize(0),_obj(NULL),_cmd(""),_str("") 
                                                { *this = src;          } 
        ~configItem()                           { clearAll();           }

        CtrlPtr obj()                           { return _obj;          }
        String cmd()                            { return _cmd;          }
        String str()                            { return _str;          }
        uint32_t size()                         { return _dataSize;     }
        uint8_t * pData()                       { return _pData;        }
        uint32_t param(uint8_t i){                       
            if (i < 4){
                return _param[i];
            } 
            return 0;
        }
        uint8_t data(uint32_t i){
            if (i < _dataSize){
                return _pData[i];
            }
            return 0;
        }


        configItem(JsonDocument config){
            _obj = objNameToPtr((const char*) config["obj"]);
            _cmd = (const char *) config["cmd"]  ;
            _str = (const char *) config["str"]  ;
            // fix NULL pointer return
            if (_cmd.c_str() == NULL)   _cmd = "";  
            if (_str.c_str() == NULL)   _str = "";

            String value;
            for(int i=0;i<4;i++){
                _param[i] = (uint32_t) convertStrToInt( (const char *) config["param"][i] );
            }

            JsonArray arr = config["bin"].as<JsonArray>();
            if (arr == nullptr){
                // no bin data
                _pData = NULL;
                _dataSize = 0;
            } else {
                int count = arr.size();
                _dataSize = count;
                _pData = new uint8_t[count];
                if (_pData == NULL){
                    _dataSize = 0;
                } else {
                    uint8_t *p = _pData;
                    for (auto value : arr) {
                        *p = value.as<unsigned char>();
                        p++;
                    }
                }
            }
        }


        bool assignData(uint8_t * pSrc,uint32_t size){
            deleteExistingData();
            if (size == 0)      return true;
            if (pSrc == NULL)   return true;
            _pData = new uint8_t[size];
            if (_pData == NULL) return false;
            memcpy(_pData,pSrc,size);
            _dataSize = size;
            return true;
        }

        void deleteExistingData(){
            if (_pData != NULL){
                delete _pData;
                _pData = NULL;
            }
            _dataSize = 0;
        }

        void clearParameter(){
            _obj = NULL;
            _cmd = "";
            _str = "";
            _param[0] = _param[1] = _param[2] = _param[3] = 0;
        }  

        void clearAll(){
            deleteExistingData();
            clearParameter();
        }



        void operator=(const configItem & src){
            _obj     = src._obj;
            _cmd     = src._cmd;
            _str     = src._str;
            for(int i=0;i < 4;i++)  _param[i]=src._param[i];
            assignData(src._pData,src._dataSize);
        }

        bool run(){
            if (_obj != NULL){
                if (_obj->select(_cmd.c_str()) == ANI_OK){
                    _obj->config(AniCfg(_param[0],_param[1],_param[2],_param[3],_str));
                    return true;
                }
            }
            return false;
        }
 
    protected:
        CtrlPtr     _obj;
        String      _cmd,_str;
        uint32_t    _param[4];
        uint8_t     * _pData;
        uint32_t    _dataSize;
};

