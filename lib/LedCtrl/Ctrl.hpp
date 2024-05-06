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
        Ctrl():_aniNameList("")         {
            _mutexSetup.lock();
                _currentNode.nr      = -1;
                _currentNode.pAni    = NULL;
                _currentNode.pName   = "";
            _mutexSetup.free();
        }

        ~Ctrl()                         {
            _mutexSetup.lock();
                _currentNode.nr      = -1;
                _currentNode.pAni    = NULL;
                _currentNode.pName   = "";
                _aniList.clear();       // clear List
                _aniNameList = "";      // clear Name List
            _mutexSetup.free();
        }

        uint32_t            getAniCount()   { return _aniList.size();                       }
        const char *        getNameList()   { return (const char *)_aniNameList.c_str();    }       // like :"0:xxx;1:yyy;..."
        virtual const char* getName()       { return _currentNode.pName;                    }
        virtual int         getNr()         { return _currentNode.nr;                       } 
        virtual bool        isAniSelected() { return (_currentNode.nr == -1) ? false:true;  }

        void addAni(Ani * pAni){
            ASSERT(pAni != NULL,"");
            if (pAni == NULL) return;

            // collect new data
            struct AniNode newNode;
            newNode.pAni  = pAni;
            newNode.nr    = _aniList.size();
            newNode.pName = pAni->getName();

            // add new entry to list
            _aniList.add(newNode);

            // update name list
            if (_aniNameList.length() > 0){
                _aniNameList += ',';
            }
            _aniNameList += String(newNode.nr);
            _aniNameList += ':';
            _aniNameList += newNode.pName;
        }

        int select(int nr){
            if (_aniList.size()==0)     return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
            if (nr >= _aniList.size())  return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
            if (nr < 0 )                return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
            // find matching entry or set all to NULL
            _mutexSetup.lock();
                _currentNode = _aniList.get(nr);
                _currentNode.pAni->reset();
            _mutexSetup.free();
            return ANI_OK;
        }

        int select(const char * pName){
            if (_aniList.size() == 0)  return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
            _mutexSetup.lock();
                // find matching entry or set all to NULL
                AniNode elm;
                elm.nr = -1;
                for(int i=0;i < _aniList.size(); i++){
                    elm = _aniList.get(i);
                    if (strcmp(pName,elm.pName) == 0)   break;  // machting entry found
                }

                if (elm.nr == -1){
                    // no matching element found
                    _mutexSetup.free();
                    return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
                }   
                _currentNode = elm;
                _currentNode.pAni->reset();
            _mutexSetup.free();
            return ANI_OK;
        }



        int config(AniCfg cfg){
            if (_currentNode.nr < 0)   return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
            _mutexSetup.lock();
                int res = _currentNode.pAni->config(cfg);
            _mutexSetup.free();
            return res;
        }


    
    protected:
        SimpleList<AniNode>     _aniList;
        AniNode                 _currentNode;
        String                  _aniNameList;   // "0:OFF;1:Static;2:Blink"  ....

        // this objects will be used from two cores 
        // so there must be an access control to local parameters to avoid race conditions
        Mutex       _mutexSetup;
};
