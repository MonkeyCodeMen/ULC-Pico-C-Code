
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
#include <Debug.hpp>

class Split
{
    public:
        Split(char * pList, char sep)           {_init(pList,sep);                                          }
        Split(char * pList, char * sepStr)      {_init(pList,sepStr);                                       }

        Split(String& list,char sep)            {_init((char *)(list.c_str()),sep);                         }
        Split(String& list,char * sepStr)       {_init((char *)(list.c_str()),sepStr);                      }
        Split(String& list,String& sepStr)      {_init((char *)(list.c_str()),(char *)(sepStr.c_str()));    }
        Split(char * pList,String& sepStr)      {_init(pList,(char *)(sepStr.c_str()));                     }

        ~Split() = default;

        void   rewind();
        bool   isEndReached()     {return _endReached;}
        String getNextListEntry();

    private:
        void    _init(char * pList, char sep);
        void    _init(char * pList, char * sepStr);
        int     _findNextSep(uint32_t startPos);
        int     _findNextSepStr(uint32_t startPos);
        String  _getNextWithSepChar();
        String  _getNextWithSepStr();
        char *  _text;
        char    _sep;
        char *  _sepStr;
        int     _sepLen;
        int     _len;
        int     _nextStart;
        bool    _endReached;
        bool    _sepIsString;
};

