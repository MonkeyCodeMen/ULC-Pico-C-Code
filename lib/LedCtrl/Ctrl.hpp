
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
