#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include "helper.hpp"


class ColorSelector{
    #define COLOR_LIST_LENGTH 32
    public:
        ColorSelector() {  setup();    };       // setup with default values 
        ~ColorSelector() = default;

        void setup(u8_t dimValue=255,u8_t startValue=0,u8_t incStep=1,String str="",u32_t length=0,u8_t * pData=NULL){
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
        };

        u32_t getNextColor(){
            u32_t color24;
            if (_useColorList){
                color24 = _colorList[_index];
                _index += _incStep;
                _index %= _colorCount;

            } else {
                color24 = getColorWheel24Bit(_index);
                _index += _incStep;
            }
            return dimColor255(color24,_dimValue);            
        };

        void setDim(u8_t dim)           {_dimValue = dim;};
        void setStep(u8_t step)         {_incStep = step;};
        u32_t getNextColor(u8_t dim)    { setDim(dim); return getNextColor();};

        void backup(){
            _backupDim  = _dimValue;
            _backupInc  = _incStep;
            _backupIndex= _index;
        };

        void restore(){
            _dimValue = _backupDim;
            _incStep  = _backupInc;
            _index    = _backupIndex;
        };


    private:
        u8_t  _dimValue,_incStep,_index,_colorCount;
        u8_t  _backupDim,_backupInc,_backupIndex;
        bool _useColorList;
        u32_t _colorList[COLOR_LIST_LENGTH];

        void _clearList(){
            _colorCount = 0;
            memset(_colorList,0,sizeof(_colorList));
        };

        void _decodeAsciiColorList(const char * listText){
            StringList list(listText,',');
            while(list.isEndReached() == false){
                _colorList[_colorCount] = convertStrToInt(list.getNextListEntry());
                _colorCount++;
            }
        };

        void _decodeBinColorList(u8_t * pData,u32_t length){
            if (length % 4 != 0){
                LOG(F_CONST("_decodeBinColorList  invalid length (%4 = 0!!) list skipped"));
                return;
            }

            _colorCount = length/4;
            if (_colorCount > COLOR_LIST_LENGTH){
                _colorCount = COLOR_LIST_LENGTH;
                LOG(F_CONST("_decodeBinColorList  list to long, list truncated"));
            }

            u32_t * p=(u32_t*)pData;
            for(int i=0;i < _colorCount;i++){
                _colorList[i] = *p;
                p++;
            }
        };

};