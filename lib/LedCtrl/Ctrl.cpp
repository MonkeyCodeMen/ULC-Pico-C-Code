#include "Ctrl.hpp"
#include "Debug.hpp"
#include "helper.h"

Ctrl::Ctrl():_aniNameList("")
{
    _mutexSetup.lock();

    _currentNode.nr      = -1;
    _currentNode.pAni    = NULL;
    _currentNode.pName   = "";

    _mutexSetup.free();
}


Ctrl::~Ctrl()
{
    _mutexSetup.lock();
    
    _currentNode.nr      = -1;
    _currentNode.pAni    = NULL;
    _currentNode.pName   = "";

    // clear List
    _aniList.clear();

    // clear Name List
    _aniNameList = "";

    _mutexSetup.free();
}

void Ctrl::addAni(Ani * pAni){
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


int Ctrl::setup(int nr){
//    if (_aniList.size()==0)     return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    if (nr >= _aniList.size())  return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    if (nr < 0 )                return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;

    // find matching entry or set all to NULL
    _mutexSetup.lock();

    _currentNode = _aniList.get(nr);
    _currentNode.pAni->reset();
    
    _mutexSetup.free();
    return ANI_OK;
}

int Ctrl::setup(const char * pName){
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



int Ctrl::setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData){
    if (_currentNode.nr < 0)   return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;

    _mutexSetup.lock();
    int res = _currentNode.pAni->setup(p1,p2,p3,p4,str,length,pData);
    _mutexSetup.free();
    return res;
}

