#pragma once

#include <Arduino.h>
#include <LedObjects.hpp>
#include <SimpleList.hpp>
#include <helper.h>
#include <ArduinoJson.h>


/****
 * 
 * JSON config file for configItem
 * 
 * {
 *      "group" : "LED0",
 *      "program": "off",
 *      "param": ["0x00","","",""],
 *      "str":"",
 *      "bin":[0,2,255,40,30]
 * }
 * 
*/



class configItem{
    public:
        configItem():_pData(NULL),_dataSize(0)  { clearParameter();     }
        configItem(configItem& src)             { *this = src;          }
        ~configItem()                           { clearAll();           }

        configItem(JsonDocument config){
            _ctrlObj = objNametoPtr((const char*) config["group"]);
            _cmd = String((const char*) config["program"]);
            _str = String((const char*) config["str"]);
            String value;
            for(int i=0;i<4;i++){
                _param[i] = convertStrToInt( (const char *) config["param"][i] );
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
                _dataSize = 0;
            }
        }

        void clearParameter(){
            _ctrlObj = NULL;
            _cmd = "";
            _param[0] = _param[1] = _param[2] = _param[3] = 0;
            _str = "";
        }  

        void clearAll(){
            deleteExistingData();
            clearParameter();
        }



        configItem& operator=(const configItem& src){
            _cmd     = src._cmd;
            _str     = src._str;
            _ctrlObj = src._ctrlObj;
            for(int i=0;i < 4;i++)  _param[i]=src._param[i];
            assignData(src._pData,src._dataSize);
            return *this;
        }

        CtrlPtr     _ctrlObj;
        String      _cmd,_str;
        uint32_t    _param[4];
        uint8_t     * _pData;
        uint32_t    _dataSize;
};

