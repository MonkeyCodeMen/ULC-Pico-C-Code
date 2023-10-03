#pragma once

class Mutex
{
public:
	Mutex();
	~Mutex() = default;

    void lock();    
    void unlock();
    bool isLocked();

private:
    volatile bool _available;

};
