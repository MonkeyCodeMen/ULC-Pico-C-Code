#include "StringList.hpp"
#include "Debug.hpp"

StringList::StringList(const char * pList, char sep):_text(pList),_sep(sep),_len(0),_nextStart(0),_endReached(false)
{
    _len = strlen(_text);
    if (_len==0){
        _endReached = true;
    }
}

void StringList::rewind(){
    _len = strlen(_text);
    if (_len==0){
        _endReached = true;
    } else {
        _endReached = false;
    }
    _nextStart = 0;
}

int StringList::_findNextSep(u32_t startPos){
    u32_t pos = startPos;
    while(pos < _len){
        if (_text[pos] == _sep){
            return pos;
        }
        pos++;
    }
    return -1; // no more spe found
}
    

String StringList::getNextListEntry(){
    String res="";
    if (_endReached == true){
        return res;
    }

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
