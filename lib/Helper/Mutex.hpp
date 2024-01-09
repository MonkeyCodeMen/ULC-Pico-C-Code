#pragma once

class Mutex
{
    public:
        Mutex():_available(true) {}
        ~Mutex() = default;

        void lock()             { while(_available == false){};    _available = false;    }
        void free()             { _available = true;  }
        bool isLocked()         { return (_available==true)?false:true;}

    private:
        volatile bool _available;

};
