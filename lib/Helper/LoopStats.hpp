#pragma once

#include <Arduino.h>

class LoopStats
{
    public:
        LoopStats(u8_t bands, u32_t factor_ms);
        ~LoopStats() = default;

        void measure(u32_t now);
        void reset();
        void print();
    
    private:
        u16_t **    _pOccurence;
        u16_t       _measures;
        u32_t       _min,_max,_mean;
        u32_t       _bands;
        u32_t       _factor_ms;
        
};
