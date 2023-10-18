#pragma once
#include <Arduino.h>

class StringList
{
    public:
        StringList(const char * pList, char sep);
        //StringList(String& in,char sep)  {StringList((const char *)in.c_str(),sep);};
        ~StringList() = default;

        void   rewind();
        bool   isEndReached()     {return _endReached;};
        String getNextListEntry();

    private:
        int     _findNextSep(u32_t startPos);
        const char *    _text;
        char            _sep;
        int             _len;
        int             _nextStart;
        bool            _endReached;
};
