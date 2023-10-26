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
    _min = 0;
    _max = 0xFFFFFFFF;
    _sum = 0;
}


void LoopStats::measure(u32_t now){
    static u32_t lastMeasure = 0;
    if (_sampleCount < _sampleTarget){
        u32_t diff = (now-lastMeasure) / _factor_ms;
        lastMeasure = now;
        diff = clamp(0,diff,_bands);
        _pBuffer[diff]++;
        _sampleCount++;
        _sum += diff;
        _min = min(diff,_min);
        _max = max(diff,_max);
    }
}


String LoopStats::print(){
    String out = "";
    for(int i=0;i < _bands; i++){
      out += "duration "+String(i*_factor_ms)+" to "+String((i+1)*_factor_ms)+" ms: "+String(_pBuffer[i])+"times \n";
    }
    out += "duration more than "+String(_bands*_factor_ms)+" ms: "+String(_pBuffer[_bands])+"times \n";
    out += "\n";
    out += "mean : "+String(_sum/_sampleCount*_factor_ms)+" ms";
    out += "min  : "+String(_min*_factor_ms)+" ms";
    out += "max  : "+String(_max*_factor_ms)+" ms";

    return out;
}
      