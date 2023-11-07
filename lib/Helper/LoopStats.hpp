#pragma once

#include <Arduino.h>

class LoopStats
{
    public:
        LoopStats(u8_t bands, u32_t factor_ms);
        ~LoopStats() = default;

        void measure(u32_t now);
        void measureAndPrint(u32_t now,u32_t printEachMs,const char * text);
        void reset();
        String print();
        u16_t getSampleCount()          {return _sampleCount;};
        void limitSamples(u16_t count)  {_sampleTarget = count;};
        bool sampleDone()               { return _sampleCount >= _sampleTarget ? true : false;};

    
    private:
        u16_t *     _pBuffer;
        u16_t       _sampleCount,_sampleTarget;
        u32_t       _min,_max,_sum;
        u32_t       _bands;
        u32_t       _factor_ms;
        bool        _first;
        u32_t       _lastCall;
        u32_t       _lastStatReport;
        
};
