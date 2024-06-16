#include "StringList.hpp"

StringList::StringList(const char * pList, char sep):
_text(pList),_sep(sep),_len(0),_nextStart(0),_endReached(false),_sepIsString(false)
{
    _len = strlen(_text);
    if (_len==0){
        _endReached = true;
    }
}

StringList::StringList(const char * pList, const char * sepStr):
_text(pList),_len(0),_nextStart(0),_endReached(false),_sepIsString(true),_sepStr(sepStr)
{
    _len    = strlen(_text);
    _sepLen = strlen(_sepStr);
    if ((_len==0) || (_sepLen == 0) ){
        _endReached = true;
    }
}

void StringList::rewind(){
    _len = strlen(_text);
    if ((_len==0) || ((_sepIsString == true) && (_sepLen == 0))){
        _endReached = true;
    } else {
        _endReached = false;
    }
    _nextStart = 0;
}

int StringList::_findNextSep(uint32_t startPos){
    int pos = startPos;
    while(pos < _len){
        if (_text[pos] == _sep){
            return pos;
        }
        pos++;
    }
    return -1; // no more sep found
}

int StringList::_findNextSepStr(uint32_t startPos){
    int pos = startPos;
    while(pos+_sepLen <= _len){
        if (strncmp(&_text[pos],_sepStr,_sepLen) == 0){
            return pos;
        }
        pos++;
    }
    return -1; // no more sep found
}

String StringList::getNextListEntry(){
    if (_endReached == true){
        String res="";
        return res;
    }

    if (_sepIsString == true){
        return _getNextWithSepStr();
    }

    return _getNextWithSepChar();
}

String  StringList::_getNextWithSepChar(){
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

String  StringList::_getNextWithSepStr(){
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
