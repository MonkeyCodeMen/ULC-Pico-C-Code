#pragma once

#include <Arduino.h>
#include "Ani.hpp"
#include "Mutex.hpp"


struct Node{
    struct Node *   pNext;
    u32_t           nr;
    const char  *   pName;    
    Ani         *   pAni; 
};

class Ctrl
{
    public:
        Ctrl();
        ~Ctrl();

        virtual String  getName(); 
        virtual void setup(int nr);         // change programm
        virtual void setup(const char * pName);
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData); // config program
        virtual void loop(u32_t time) {};           // update LED's (derived type)
        void addAni(Ani * pAni);
        u32_t getAniCount()         {return _count;};
        String getNameList()        {return _aniNameList;};       // like :"0:xxx;1:yyy;..."
    
    protected:
        struct Node * _pRoot;
        struct Node * _pCurrentNode;
        Ani         * _pCurrentAni;
        String      _aniNameList;   // "0:OFF;1:Static;2:Blink"  ....
        u32_t       _count;


        // this objects will be used from two cores 
        // core 0 : communication & menue
        // core 1 : Led's  
        // so there must be an access control to local parameters to avoid race conditions
        Mutex       _mutexSetup;

};
