#pragma once

#include <Arduino.h>
#include "Ani.hpp"


struct Node{
    struct Node *   pNext;
    u32_t           nr;
    String          name;    
    Ani         *   pAni; 
};

class Ctrl
{
    public:
        Ctrl();
        ~Ctrl();

        String getNameList();       // like :"0:xxx;1:yyy;..."
        void setup(int nr);
        void setup(String name);
        void setup(int nr,u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData);
        void setup(String name,u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData);
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData);
        void reset();
        virtual void update() {};
        

    
    protected:
        void takeSemaBlocking()             {while(_sema != free); _sema=taken;};
        void freeSema()                     { _sema = free;};

        struct Node * _pRoot;
        struct Node * _pCurrentNode;
        Ani         * _pCurrentAni;
        String      _aniNameList;   // "0:OFF;1:Static;2:Blink"  ....
        u32_t       _count;

        // this objects will be used from two cores 
        // core 0 : communication & menue
        // core 1 : Led's  
        // so there must be an access control to local parameters to avoid race conditions
        // a simple blocking semaphore should be OK, may cause small glitches on LED'S but this accepatble
        enum SemaState {free,taken};
        volatile u8_t        _sema;


        void addAni(Ani * pAni);

};
