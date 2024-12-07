
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once
#include <Arduino.h>
#include <RTClib.h>

/* 
    TimeSpan is nice but i want to have more overloading operators 
*/
class TimeSpanExt:public TimeSpan {
    public:
        TimeSpanExt(int32_t seconds = 0)  : TimeSpan(seconds) {}
        TimeSpanExt(int16_t days, int8_t hours, int8_t minutes, int8_t seconds) : TimeSpan(days,hours,minutes,seconds) {}
        TimeSpanExt(const TimeSpan &copy) : TimeSpan(copy) {}
        TimeSpanExt(const TimeSpanExt &copy) : TimeSpan(copy) {}

        bool operator>(const TimeSpan &other)  {return this->_seconds  >  other.totalseconds(); }
        bool operator<(const TimeSpan &other)  {return this->_seconds  <  other.totalseconds(); }
        bool operator>=(const TimeSpan &other) {return this->_seconds  >= other.totalseconds(); }
        bool operator<=(const TimeSpan &other) {return this->_seconds  <= other.totalseconds(); }
        bool operator==(const TimeSpan &other) {return this->_seconds  == other.totalseconds(); }
        bool operator!=(const TimeSpan &other) {return this->_seconds  != other.totalseconds(); }
        
        TimeSpanExt operator+(const TimeSpanExt& other) const {
            if (_seconds > (INT32_MAX - other._seconds)) {
               return TimeSpan(INT32_MAX);
            }
            return TimeSpanExt(_seconds + other._seconds);    
        }

        TimeSpanExt operator-(const TimeSpanExt& other) const {
            int result = _seconds - other._seconds;
            if (result < 0) {
                return TimeSpanExt(0);
            }
            return TimeSpanExt(result);
        }

        TimeSpanExt operator*(int factor) const {
            if (factor <= 0) {
                return TimeSpanExt(0);
            }
            if (_seconds > (INT32_MAX / factor)){
                return TimeSpan(INT32_MAX);
            }
            return TimeSpanExt(_seconds * factor);
        }

        TimeSpanExt operator/(int divisor) const {
            if (divisor == 0) { return TimeSpanExt(INT32_MAX); }
            if (divisor <  0) { return TimeSpanExt(_seconds / (divisor * (-1))); }
            return TimeSpanExt(_seconds / divisor);
        }

        /*
         Create ISO norm time span string : "P230D2H37M42S"
         for 0 : "P0S"
        */
        String print(){
            if (totalseconds() == 0) { return String("P0S");   } 
            String out="P";
            if (days() != 0)    out+=String(days())    +'D';
            if (hours() != 0)   out+=String(hours())   +'H';
            if (minutes() != 0) out+=String(minutes()) +'M';
            if (seconds() != 0) out+=String(seconds()) +'S';
            return out;
        }
};

