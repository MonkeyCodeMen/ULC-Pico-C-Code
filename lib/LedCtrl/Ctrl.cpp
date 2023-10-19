#include "Ctrl.hpp"
#include "Debug.hpp"
#include "helper.hpp"

Ctrl::Ctrl()
{
    _pCurrentAni    = NULL;
    _pCurrentNode   = NULL;
    _pRoot          = NULL;
    _aniNameList    = "";
    _count = 0;
}


Ctrl::~Ctrl()
{
    struct Node * pNext;
    _pCurrentAni    = NULL;
    _pCurrentNode   = NULL;
    _count          = 0;

    // delete List
    while(_pRoot != NULL){
        pNext = _pRoot->pNext;
        delete(_pRoot);
        _pRoot = pNext; 
    }

    // clear Name List
    _aniNameList = "";
}

void Ctrl::addAni(Ani * pAni){
    struct Node * pNewNode;
    ASSERT(pAni != NULL,"");
    if (pAni == NULL) return;
    pNewNode = new Node;
    ASSERT(pNewNode != NULL,"");
    if (pNewNode != NULL){
        // fill new element
        pNewNode->pAni = pAni;
        pNewNode->nr   = _count;
        _count++;
        pNewNode->pName = pAni->getName();
        pNewNode->pNext= NULL;

        // update name list
        if (_aniNameList.length() > 0){
            _aniNameList += ',';
        }
        _aniNameList += String(pNewNode->nr);
        _aniNameList += ':';
        _aniNameList += pNewNode->pName;
        

        // add new element to list
        struct Node * pLast;

        if (_pRoot == NULL){
            // first entry
            _pRoot = pNewNode;
        } else {
            // serach Last element
            pLast = _pRoot;
            while (pLast->pNext != NULL){
                pLast = pLast->pNext;
            }
            // and connect new one
            pLast->pNext = pNewNode;
        }
    }
}


const char * Ctrl::getName(){
    ASSERT(_pCurrentAni != NULL,"");
    if (_pCurrentAni == NULL)   return F_CONST("");

    return _pCurrentNode->pName;
}

void Ctrl::setup(int nr){
    if (nr >= _count)        return;
    if (nr < 0 )             return;
    if (_pRoot == NULL)      return;

    _mutexSetup.lock();
    // find matching entry or set all to NULL
    _pCurrentNode = _pRoot;
    while (_pCurrentNode->nr != nr){
        if (_pCurrentNode->pNext == NULL){
            _pCurrentAni = NULL;
            _pCurrentNode= NULL;
            return;
        }
        _pCurrentNode = _pCurrentNode->pNext;
    }
    _pCurrentAni = _pCurrentNode->pAni;
    _pCurrentAni->reset();
    _mutexSetup.unlock();
}

void Ctrl::setup(const char * pName){
    if (_pRoot == NULL)      return;

    _mutexSetup.lock();
    // find matching entry or set all to NULL
    _pCurrentNode = _pRoot;
    while (strcmp(_pCurrentNode->pName,pName) != 0 ){
        if (_pCurrentNode->pNext == NULL){
            _pCurrentAni = NULL;
            _pCurrentNode= NULL;
            return;
        }
        _pCurrentNode = _pCurrentNode->pNext;
    }
    _pCurrentAni = _pCurrentNode->pAni;
    _pCurrentAni->reset();
    _mutexSetup.unlock();
}



void Ctrl::setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData){
    ASSERT(_pCurrentAni != NULL,"");
    if (_pCurrentAni == NULL)   return;

    _mutexSetup.lock();
    _pCurrentAni->setup(p1,p2,p3,p4,length,pData);
    _mutexSetup.unlock();
}

