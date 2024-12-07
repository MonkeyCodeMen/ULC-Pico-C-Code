
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


#include "Split.hpp"

void Split::_init(char * pList, char sep){
    _text=pList;
    _sep=sep;
    _sepStr = nullptr;
    _nextStart=0;
    _endReached=false;
    _sepIsString=false;
    _len = strlen(_text);
    if (_len==0){
        _endReached = true;
    }
}

void Split::_init(char * pList, char * sepStr){
    _text=pList;
    _sep='X';
    _sepStr = sepStr;
    _nextStart=0;
    _endReached=false;
    _sepIsString=true;
    _len    = strlen(_text);
    _sepLen = strlen(_sepStr);
    if ((_len==0) || (_sepLen == 0) ){
        _endReached = true;
    }
}

void Split::rewind(){
    _len = strlen(_text);
    if ((_len==0) || ((_sepIsString == true) && (_sepLen == 0))){
        _endReached = true;
    } else {
        _endReached = false;
    }
    _nextStart = 0;
}

int Split::_findNextSep(uint32_t startPos){
    int pos = startPos;
    while(pos < _len){
        if (_text[pos] == _sep){
            return pos;
        }
        pos++;
    }
    return -1; // no more sep found
}

int Split::_findNextSepStr(uint32_t startPos){
    int pos = startPos;
    while(pos+_sepLen <= _len){
        if (strncmp(&_text[pos],_sepStr,_sepLen) == 0){
            return pos;
        }
        pos++;
    }
    return -1; // no more sep found
}

String Split::getNextListEntry(){
    if (_endReached == true){
        String res="";
        return res;
    }

    if (_sepIsString == true){
        return _getNextWithSepStr();
    }

    return _getNextWithSepChar();
}

String  Split::_getNextWithSepChar(){
    String res="";
    int from = _nextStart;
    if (from >= _len){
        // last char of string was a seperator ==> one more empty entry but than finished
        _endReached = true;
        return res;
    }

    int to = _findNextSep(from);
    if (to == -1){
        // last or only entry
        _endReached = true;
        for(int i=from;i<_len;i++)    {
            res.concat(_text[i]);
        }
        return res;
    }

    if (from == to){
        // empty entry
        _nextStart=to+1;
        return res;
    } 

    ASSERT(from < to,F("StringList::copyNextListEntry   order wrong"));
    for(int i=from;i<to;i++)    {
        res.concat(_text[i]);
    }
    _nextStart=to+1;
    return res;
}

String  Split::_getNextWithSepStr(){
    String res="";
    int from = _nextStart;
    if ((from+_sepLen >= _len)  && (from != 0)){
        // last part of string was a seperator ==> one more empty entry but than finished
        _endReached = true;
        return res;
    }

    int to = _findNextSepStr(from);
    if (to == -1){
        // last or only entry
        _endReached = true;
        for(int i=from;i<_len;i++)    {
            res.concat(_text[i]);
        }
        return res;
    }

    if (from == to){
        // empty entry
        _nextStart=to+_sepLen;
        return res;
    } 

    ASSERT(from < to,F("StringList::copyNextListEntry   order wrong"));
    for(int i=from;i<to;i++)    {
        res.concat(_text[i]);
    }
    _nextStart=to+_sepLen;
    return res;
}
