#include "Mutex.hpp"

Mutex::Mutex():_available(true)
{
}


void Mutex::lock(){
    while(_available == false);
    _available = false;
}

void Mutex::free(){
    _available = true;
}

bool Mutex::isLocked(){
    if (_available == true){
        return false;
    }
    return true;
}