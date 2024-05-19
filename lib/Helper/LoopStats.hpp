#pragma once

#include <Arduino.h>
#include <Debug.hpp>
#include <helper.h>

class LoopStats
{
    public:
        LoopStats(uint8_t bands, uint32_t factor_ms):
        _sampleCount(0),_sampleTarget(0x8000),_bands(bands),_factor_ms(factor_ms){
                _pBuffer      = new uint16_t[bands+1];
                ASSERT(_pBuffer != NULL,F("loopStats could not create measurement buffer"));
                reset();
            }

        ~LoopStats() = default;
        uint16_t getSampleCount()               { return _sampleCount;                                  }
        void limitSamples(uint16_t count)       { _sampleTarget = count;                                }
        bool sampleDone()                       { return _sampleCount >= _sampleTarget ? true : false;  }

        void measure(uint32_t now){
                if (_first == true){
                    _first = false;
                    _lastCall = now;
                }
                if (_sampleCount < _sampleTarget){
                    uint32_t diff = now-_lastCall;
                    _min = min(diff,_min);
                    _max = max(diff,_max);
                    _sampleCount++;
                    _sum += diff;
                    _lastCall = now;
                    diff = diff / _factor_ms;
                    diff = clampUint32(0,diff,_bands);
                    _pBuffer[diff]++;
                }
            }

        void measureAndPrint(uint32_t now,uint32_t printEachMs,const char * text){
                measure(now);
                if (now - _lastStatReport > printEachMs){
                    String out = text;
                    out+='\n';
                    out+=print();
                    LOG(out.c_str());
                    _lastStatReport = now;
                }
            }

        void reset() {
                _sampleCount = 0;
                memset(_pBuffer,0,sizeof(uint16_t[_bands+1]));
                _min = 0xFFFFFFFF;
                _max = 0x0;
                _sum = 0;
                _lastCall = 0;
                _first = true;
            }
        String print(){
                String out = "\n\n";
                for(int i=0;i < _bands; i++){
                    out += "duration "+String(i*_factor_ms)+" to "+String((i+1)*_factor_ms)+" ms: "+String(_pBuffer[i])+"times \n";
                }
                out += "duration more than "+String(_bands*_factor_ms)+" ms: "+String(_pBuffer[_bands])+"times \n";
                out += "\n";
                out += "mean : "+String(_sum/_sampleCount)+" ms\n";
                out += "min  : "+String(_min)+" ms\n";
                out += "max  : "+String(_max)+" ms\n";
                return out;
            }

    
    private:
        uint16_t       _sampleCount,_sampleTarget;
        uint32_t       _bands;
        uint32_t       _factor_ms;
        bool            _first;
        uint32_t       _min,_max,_sum;
        uint32_t       _lastCall;
        uint32_t       _lastStatReport;
        uint16_t *     _pBuffer;
        
};

