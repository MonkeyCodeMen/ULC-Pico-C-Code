#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include "helper.h"


class ColorSelector{
    #define COLOR_LIST_LENGTH 32
    public:
        ColorSelector() {  setup();    };       // setup with default values 
        ~ColorSelector() = default;

        void setup(uint8_t dimValue=255,uint8_t startValue=0,uint8_t incStep=1,String str="",uint32_t length=0,uint8_t * pData=NULL){
            _clearList();
            _dimValue = dimValue;
            _incStep = incStep;
            if ((str.length() > 0) || (length > 0)){
                _useColorList = true;
                if (str.length() != 0){
                    _decodeAsciiColorList(str.c_str());
                } else if (length != 0){
                    _decodeBinColorList(pData,length);
                } 
                _index   = startValue%_colorCount;
            } else {
                _useColorList = false;
                _colorCount   = 255;
                _index        = startValue;
            }
            backup();
        }

        uint32_t getNextColor(){
            uint32_t color24;
            if (_useColorList){
                color24 = _colorList[_index];
                _index += _incStep;
                _index %= _colorCount;

            } else {
                color24 = getColorWheel24Bit(_index);
                _index += _incStep;
            }
            return dimColor255(color24,_dimValue);            
        }

        void setDim(uint8_t dim)           {_dimValue = dim;};
        void setStep(uint8_t step)         {_incStep = step;};
        uint32_t getNextColor(uint8_t dim)    { setDim(dim); return getNextColor();};

        void backup(){
            _backupDim  = _dimValue;
            _backupInc  = _incStep;
            _backupIndex= _index;
        }

        void restore(){
            _dimValue = _backupDim;
            _incStep  = _backupInc;
            _index    = _backupIndex;
        }


    private:
        uint8_t  _dimValue,_incStep,_index,_colorCount;
        uint8_t  _backupDim,_backupInc,_backupIndex;
        bool _useColorList;
        uint32_t _colorList[COLOR_LIST_LENGTH];

        void _clearList(){
            _colorCount = 0;
            memset(_colorList,0,sizeof(_colorList));
        }

        void _decodeAsciiColorList(const char * listText){
            StringList list(listText,',');
            while(list.isEndReached() == false){
                _colorList[_colorCount] = convertStrToInt(list.getNextListEntry());
                _colorCount++;
            }
        }

        void _decodeBinColorList(uint8_t * pData,uint32_t length){
            if (length % 4 != 0){
                LOG(F_CHAR("_decodeBinColorList  invalid length (%4 = 0!!) list skipped"));
                return;
            }

            _colorCount = length/4;
            if (_colorCount > COLOR_LIST_LENGTH){
                _colorCount = COLOR_LIST_LENGTH;
                LOG(F_CHAR("_decodeBinColorList  list to long, list truncated"));
            }

            uint32_t * p=(uint32_t*)pData;
            for(int i=0;i < _colorCount;i++){
                _colorList[i] = *p;
                p++;
            }
        }
};