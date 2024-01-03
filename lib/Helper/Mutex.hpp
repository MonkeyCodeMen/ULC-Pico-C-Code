#pragma once

class Mutex
{
public:
	Mutex();
	~Mutex() = default;

    void lock();    
    void free();
    bool isLocked();

private:
    volatile bool _available;

};
