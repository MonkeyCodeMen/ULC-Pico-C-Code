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

        virtual uint32_t    getAniCount()   { return _aniList.size();                       }
        virtual const char* getNameList()   { return (const char *)_aniNameList.c_str();    }       // like :"0:xxx;1:yyy;..."
        virtual const char* getName()       { return _currentNode.pName;                    }
        virtual int         getNr()         { return _currentNode.nr;                       } 
        virtual bool        isAniSelected() { return (_currentNode.nr == -1) ? false:true;  }

        virtual void addAni(Ani * pAni);
        virtual int select(int nr);
        virtual int select(const char * pName);
        virtual int config(AniCfg cfg);

        virtual int dump(String& out);                      // dump current animation
        virtual int dump(String& out,int nr);
        virtual int dump(String& out,const char * pName);
        
    
    protected:
        SimpleList<AniNode>     _aniList;
        AniNode                 _currentNode;
        String                  _aniNameList;   // "0:OFF;1:Static;2:Blink"  ....

        // this objects will be used from two cores 
        // so there must be an access control to local parameters to avoid race conditions
        Mutex       _mutexSetup;
};
