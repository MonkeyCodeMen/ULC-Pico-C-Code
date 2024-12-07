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
#include <TimeSpanExt.hpp>
#include <helper.h>

#ifndef SECONDS_PER_DAY
#define SECONDS_PER_DAY     86400L
#endif

#define SECONDS_PER_HOUR    3600L
#define SECONDS_PER_MINUTE  60L
#define MINUTES_PER_HOUR    60L
#define HOURS_PER_DAY       24L


class Time{
    public:
        Time(int8_t h, int8_t m, int8_t s)      { _sec=(h*SECONDS_PER_HOUR + m*SECONDS_PER_MINUTE + s); _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}
        Time(const int32_t sec = 0)             { _sec = sec;                                           _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}
        Time(const TimeSpan& src)               { _sec=src.totalseconds();                              _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}
        Time(const TimeSpanExt& src)            { _sec=src.totalseconds();                              _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}
        Time(const Time& src)                   { _sec=src.totalseconds();                              _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}
        Time(const DateTime& src)               { _sec=(src.hour()*SECONDS_PER_HOUR + src.minute()*SECONDS_PER_MINUTE + src.second());   _sec = wrapAround(0,_sec,SECONDS_PER_DAY-1);}

        Time operator+(const Time &other) const {return Time(_sec + other._sec);    }
        Time operator-(const Time &other) const {return Time(_sec - other._sec);    }
        Time operator*(int32_t factor)    const {return Time(_sec * factor);        }
        Time operator/(int32_t factor)    const { return (factor<=0) ? (Time(SECONDS_PER_DAY-1)) : (Time(_sec / factor));  }
        
        bool operator>(const Time &other)   {return this->_sec  >  other.totalseconds(); }
        bool operator<(const Time &other)   {return this->_sec  <  other.totalseconds(); }
        bool operator>=(const Time &other)  {return this->_sec  >= other.totalseconds(); }
        bool operator<=(const Time &other)  {return this->_sec  <= other.totalseconds(); }
        bool operator==(const Time &other)  {return this->_sec  == other.totalseconds(); }
        bool operator!=(const Time &other)  {return this->_sec  != other.totalseconds(); }

        int8_t hour() const             { return (_sec / SECONDS_PER_HOUR)   % HOURS_PER_DAY;       }
        int8_t minute() const           { return (_sec / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR;    }
        int8_t second() const           { return _sec                        % SECONDS_PER_MINUTE;  }
        int32_t totalseconds() const    { return _sec;                                              }
        void setHour(uint8_t h)         { _sec = Time(h,minute(),second()).totalseconds();          }
        void setMinute(uint8_t m)       { _sec = Time(hour(),m,second()).totalseconds();            }
        void setSec(uint8_t s)          { _sec = Time(hour(),minute(),s).totalseconds();            }

        String toFixedWidthText(char separator = ':', char leadChar = 0) const {
            char buffer[10]; //_HH:MM:SS + null terminator
            if (leadChar == 0)
                snprintf(buffer, sizeof(buffer), "%02d%c%02d%c%02d", hour(), separator, minute(), separator, second());
            else
                snprintf(buffer, sizeof(buffer), "%c%02d%c%02d%c%02d", leadChar, hour(), separator, minute(), separator, second());
            
            return String(buffer);
        }

        // Function to return time as text with variable-width fields
        String toVariableWidthText(char separator = ':', char leadChar = 0) const {
            String result="";
            if (leadChar != 0){
                result += leadChar;
            }
            result += String(hour(),DEC) + separator;
            result += String(minute(),DEC) + separator;
            result += String(second(),DEC);
            return result;
        }


    protected:
        uint32_t _sec;
};

