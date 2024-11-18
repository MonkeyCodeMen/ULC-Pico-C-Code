#include "BufferedClock.hpp"
#include <Resources.hpp>


BufferdClock bufferedClock;


void BufferdClock::begin(TwoWire *pBus, Mutex * pMutex, uint32_t syncInterval) {
    uint32_t now = millis();
    // Step 1: Check if the provided I2C bus pointer is valid
    if (pBus == nullptr) {
        LOG(F("Invalid bus"));      // Log an error if the bus pointer is invalid
        _pBus = nullptr;            // Set internal bus pointer to null
        _validRTC = false;
        return;                     // Exit the function early
    }
    _pBus = pBus;                   // Store the valid bus pointer

    // Step 2: Initialize or assign mutex
    if (pMutex == nullptr) {
        _pMutex = &_dummyMutex;     // if no Mutex provided use internal dummy Mutex
    } else {
        _pMutex = pMutex;           // Use the provided mutex
    }

    // Step 3: Adjust sync interval if it is too short
    if ((syncInterval > 0) && (syncInterval < MIN_SYNC_INTERVAL_MSEC)) {
        LOG(F("Sync interval too small ... adjusted to minimum"));
        _syncInterval = MIN_SYNC_INTERVAL_MSEC;  // Set to minimum interval
    } else {
        _syncInterval = syncInterval;            // Use the provided interval
    }

    // Step 4: Lock the mutex to ensure safe access to shared resources
    _pMutex->lock();
    _pBus->setClock(I2C_100KHZ);
    if (_RTC.begin(_pBus)) {        // Attempt to start communication with the RTC
        _startDate = _RTC.now();    // Sync software time with RTC
        _lastSync = now;            // Set the last sync time to current time
        _validRTC = true;           // Mark RTC as valid and synchronized
    } else {
        // If RTC fails, log an error and set a default date/time
        LOG(F("Could not sync software clock with RTC .. setting to 2024-12-24 11:11:11"));
        _startDate = DateTime(2024, 12, 24, 11, 11, 11);
        _validRTC = false;          // Mark RTC as invalid
        _errorCounterInit++;
    }
    _pBus->setClock(I2C_DEFAULT_SPEED);
    _pMutex->free();                // Unlock the mutex after initialization
}


void BufferdClock::loop(uint32_t now) {
    if ((now - _lastSync >= _syncInterval) && (_syncInterval != NO_SYNC)){
        // time for sync
        bool syncDone = false;
        if (_validRTC == false){
                // no successfull init until now ==> try a new init
                if (_pMutex->tryLock()){ 
                    _pBus->setClock(I2C_100KHZ);
                    if (_RTC.begin(_pBus)) {        // Attempt to start communication with the RTC
                        _startDate = _RTC.now();    // Sync software time with RTC
                        _lastSync = now;            // only in case of success set the last sync time to current time
                        _validRTC = true;           // Mark RTC as valid and synchronized                   
                        syncDone = true;
                    } else {
                        _errorCounterInit++;
                    }
                    _pBus->setClock(I2C_DEFAULT_SPEED);
                    _pMutex->free();
                } 
        } else {
            // read time from RTC and update/sync SW clock 
            if (_pMutex->tryLock() == true){
                _pBus->setClock(I2C_100KHZ);
                _startDate = _RTC.now();    // Update the base date/time from RTC
                _pBus->setClock(I2C_DEFAULT_SPEED);
                _pMutex->free();
                _lastSync   = now;          // only in case of success record the last sync time
                syncDone    = true;
            }
            // Attention: if MUTEX is haevily used by other applications a sync on regular base is not garantueed 

        }
        if (syncDone == false){
            // update was not successfull this time 
            // sync counter/timer are not reseted ==> sync will be tried next loop again
            // do for now SW update only
            _loopDate = _startDate + TimeSpan((now - _lastSync) / 1000);
        }
    } else {
        // only SW clock update .. no HW access
        _loopDate = _startDate + TimeSpan((now - _lastSync) / 1000);
    }
}


bool BufferdClock::udateRTC(DateTime newDateTime,bool blocking){
    _startDate = newDateTime;    
    _lastSync = millis();            // Set the last sync time to current time
    if (_validRTC == false){
        return false;
    }

    // wait for mutex or just try one time
    if (blocking == true){
        _pMutex->lock();
    } else {
        if (_pMutex->tryLock() == false){
            return false;
        }
    }

    _pBus->setClock(I2C_100KHZ);
    _RTC.adjust(newDateTime);
    _pBus->setClock(I2C_DEFAULT_SPEED);
    _pMutex->free();

    return true;
}