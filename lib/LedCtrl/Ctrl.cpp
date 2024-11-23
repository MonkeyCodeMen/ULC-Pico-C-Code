
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


#include <Ctrl.hpp>


Ctrl::Ctrl():_aniNameList("")         {
    _mutexSetup.lock();
        _currentNode.nr      = -1;
        _currentNode.pAni    = NULL;
        _currentNode.pName   = "";
    _mutexSetup.free();
}

Ctrl::~Ctrl()                         {
    _mutexSetup.lock();
        _currentNode.nr      = -1;
        _currentNode.pAni    = NULL;
        _currentNode.pName   = "";
        _aniList.clear();       // clear List
        _aniNameList = "";      // clear Name List
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

int Ctrl::select(int nr){
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

int Ctrl::select(const char * pName){
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

int Ctrl::config(AniCfg cfg){
    if (_currentNode.nr < 0)   return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    _mutexSetup.lock();
        int res = _currentNode.pAni->config(cfg);
    _mutexSetup.free();
    return res;
}



int Ctrl::dump(String& out){
    if ( _currentNode.pAni == NULL) return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    out += _currentNode.pAni->dump();   
    return ANI_OK;
}

        
int Ctrl::dump(String& out,int nr){
    if ( (_aniList.size()==0)  || (nr >= _aniList.size()) || (nr < 0 )){
        return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    }  
    out += _aniList.get(nr).pAni->dump();
    return ANI_OK;
}

int Ctrl::dump(String& out,const char * pName){
    if (_aniList.size()==0) return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    AniNode elm;
    elm.nr = -1;
    for(int i=0;i < _aniList.size(); i++){
        elm = _aniList.get(i);
        if (strcmp(pName,elm.pName) == 0)   break;  // machting entry found
    }

    if (elm.nr == -1){
        return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    } 

    out += elm.pAni->dump();
    return ANI_OK;
}

