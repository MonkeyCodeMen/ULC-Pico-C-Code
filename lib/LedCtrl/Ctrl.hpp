#pragma once

#include <Arduino.h>
#include "Ani.hpp"
#include "Mutex.hpp"
#include <SimpleList.hpp>



struct AniNode{
    //struct AniNode *   pNext;
    int32_t         nr;
    const char  *   pName;    
    Ani         *   pAni; 
};

class Ctrl
{
    public:
        Ctrl();
        ~Ctrl();

        virtual int setup(int nr);                 // change program / animation
        virtual int setup(const char * pName);     // change program / animation
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData); // config program
        void addAni(Ani * pAni);
        uint32_t getAniCount()          { return _aniList.size();                       }
        const char * getNameList()      { return (const char *)_aniNameList.c_str();    }       // like :"0:xxx;1:yyy;..."
        virtual const char * getName()  { return _currentNode.pName;                    }
        virtual int getNr()             { return _currentNode.nr;                       } 
        virtual bool isAniSelected()    { if (_currentNode.nr == -1) return false; return true;}
    
    protected:
        SimpleList<AniNode>     _aniList;
        AniNode                 _currentNode;
        String                  _aniNameList;   // "0:OFF;1:Static;2:Blink"  ....

        // this objects will be used from two cores 
        // so there must be an access control to local parameters to avoid race conditions
        Mutex       _mutexSetup;
};
