#include "LoopStats.hpp"
#include <Debug.hpp>
#include <helper.hpp>

LoopStats::LoopStats(u8_t bands, u32_t factor_ms)
{
    _sampleTarget = 0x8000;
    _factor_ms    = factor_ms;
    _bands        = bands;
    _pBuffer      = new u16_t[bands+1];
    ASSERT(_pBuffer != NULL,F_CONST("loopStats could not create measurement buffer"));
    reset();
}

void LoopStats::reset(){
    _sampleCount = 0;
    memset(_pBuffer,0,sizeof(u16_t[_bands+1]));
    _min = 0xFFFFFFFF;
    _max = 0x0;
    _sum = 0;
    _lastCall = 0;
    _first = true;
}


void LoopStats::measure(u32_t now){
    if (_first == true){
        _first = false;
        _lastCall = now;
    }
    if (_sampleCount < _sampleTarget){
        u32_t diff = now-_lastCall;
        _min = min(diff,_min);
        _max = max(diff,_max);
        _sampleCount++;
        _sum += diff;
        _lastCall = now;
        
        diff = diff / _factor_ms;
        diff = clamp(0,diff,_bands);
        _pBuffer[diff]++;
    }
}


String LoopStats::print(){
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
      