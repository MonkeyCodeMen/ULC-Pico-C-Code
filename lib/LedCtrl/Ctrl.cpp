#include "Ctrl.hpp"
#include "Debug.hpp"

Ctrl::Ctrl()
{
    _pCurrentAni    = NULL;
    _pCurrentNode   = NULL;
    _pRoot          = NULL;
    _aniNameList    = String("");
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
    _aniNameList = String("");
}

void Ctrl::_addAni(Ani * pAni){
    struct Node * pNewNode;

    pNewNode = new Node;
    ASSERT(pNewNode != NULL,"");
    if (pNewNode != NULL){
        // fill new element
        pNewNode->pAni = pAni;
        pNewNode->nr   = _count;
        _count++;
        pNewNode->name = pAni->getName();
        pNewNode->pNext= NULL;

        // update name list
        _aniNameList += String(pNewNode->nr);
        _aniNameList += ":";
        _aniNameList += pNewNode->name;
        _aniNameList += ";";

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


String Ctrl::getNameList(){
    return _aniNameList;
}

String Ctrl::getName(){
    ASSERT(_pCurrentAni != NULL,"");
    if (_pCurrentAni == NULL)   return String("");

    return _pCurrentNode->name;
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

void Ctrl::setup(String& name){
    if (_pRoot == NULL)      return;

    _mutexSetup.lock();
    // find matching entry or set all to NULL
    _pCurrentNode = _pRoot;
    while (_pCurrentNode->name != name){
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


