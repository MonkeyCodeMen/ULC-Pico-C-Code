#pragma once
#include <Arduino.h>
#include <Debug.hpp>

class StringList
{
    public:
        StringList(const char * pList, char sep);
        StringList(const char * pList, const char * sepStr);
        //StringList(String& in,char sep)  {StringList((const char *)in.c_str(),sep);};
        ~StringList() = default;

        void   rewind();
        bool   isEndReached()     {return _endReached;}
        String getNextListEntry();

    private:
        int     _findNextSep(uint32_t startPos);
        int     _findNextSepStr(uint32_t startPos);
        String  _getNextWithSepChar();
        String  _getNextWithSepStr();
        const char *    _text;
        char            _sep;
        const char *    _sepStr;
        int             _sepLen;
        int             _len;
        int             _nextStart;
        bool            _endReached;
        bool            _sepIsString;
};

