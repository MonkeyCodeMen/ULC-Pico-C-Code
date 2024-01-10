#pragma once
#include "Arduino.h"
#include <Debug.hpp>




/*
    very short implementation of ring buffer 
    Thread safe 
    but max capacity = nr of buffer elements - 1   ==> we loose one place by index handling 
*/

template<class T>
class RingBuffer {

private:
	uint8_t _size;
	volatile uint8_t _read,_write;
	T*  _array;

public:
	// Constructor
	RingBuffer(uint8_t size);

	// Destructor
	~RingBuffer();

	// Add element to Ring Buffer
	bool push(T value);

    // get element (and remove) from ring Buffer
    bool pop(T *  pValueDest);
    
    // getter functions
    uint8_t size() const;                // size of buffer
	uint8_t count() const;               // elements available in buffer
    uint8_t free() const;                // elements free in buffer
    inline bool isFull() const;     // buffer is completly filled (next push will fail)
    inline bool hasSpace() const;   // at least space for one more element in buffer
    inline bool isEmpty() const;    // buffer is empty 

    void clear();
};


template<class T> RingBuffer<T>::RingBuffer(uint8_t size) {
    _size = size+1;

    if (size == 0){
        STOP(F_CHAR("invalid Buffer size"));
    }
    _array = new T[_size];

    if (_array == NULL){
        STOP(F_CHAR("not enough memory for Buffer"));
    }
    // init read / write index 
    _write = 0;
    _read  = 0;
}

template<class T> RingBuffer<T>::~RingBuffer() {
    if ( _array != NULL){
        delete _array;
    }
}

template<class T> void RingBuffer<T>::clear() {
	_write = 0;
    _read  = 0;
}


template<class T> uint8_t RingBuffer<T>::size() const {
	return _size-1;
}

template<class T> bool RingBuffer<T>::isFull() const {
    if ( ( _write + 1 == _read ) || ( (_read == 0) && _write + 1 == _size ) )    {
        return true; 
    }
    return false;
}

template<class T> bool RingBuffer<T>::hasSpace() const {
    if ( ( _write + 1 == _read ) || ( (_read == 0) && _write + 1 == _size ) )    {
        return false; 
    }
    return true;
}

template<class T> bool RingBuffer<T>::isEmpty() const {
    if (_read == _write)    {
        return true; 
    }
    return false;
}

template<class T>uint8_t RingBuffer<T>::free() const{
    if (_read == _write){
        // empty
        return _size - 1; 
    } else if (_write < _read){
        return _read - _write - 1;
    } 
    return _size - (_write - _read) - 1;
}

template<class T>uint8_t RingBuffer<T>::count() const{
    return _size-free()-1;
}

template<class T> bool RingBuffer<T>::push(T value) {
    if ( isFull() ){
        return false; 
    }
        
    // add element to buffer
    _array[_write] = value;
    // inc write index
    _write++;
    // make wrap around if applicable
    if (_write >= _size){
        _write = 0;
    }

    return true;
}

template<class T> bool RingBuffer<T>::pop(T * pDestValue) {
    if ( isEmpty()  ){
        return false; 
    }
        
    // add element to buffer
    *pDestValue = _array[_read];
    // inc write index
    _read++;
    // make wrap around if applicable
    if (_read >= _size){
        _read = 0;
    }

    return true;
}


