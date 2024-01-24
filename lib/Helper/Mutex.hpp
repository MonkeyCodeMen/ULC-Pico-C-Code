#pragma once

class Mutex
{
    public:
        Mutex():_available(true) {}
        ~Mutex() = default;

        void lock()             { while(_available == false){};    _available = false;    }
        void free()             { _available = true;  }
        bool isLocked()         { return (_available==true)?false:true;}
        bool tryLock()          { 
            bool res = false;
            noInterrupts(); 
            if (_available == false) { 
                res = false;
            } else {
                _available = false;
                res = true;
            }
            interrupts();
            return res;
        }

    private:
        volatile bool _available;

};
