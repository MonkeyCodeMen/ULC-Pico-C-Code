#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Mutex.hpp>
#include <Debug.hpp>
#include <RTClib.h>

/**********************************************************************/
/*!
    @class   Time
    @brief   Manages the synchronization and updating of a software clock 
             with a hardware RTC module.
    @details This class provides methods to initialize, synchronize, and 
             update a software-based clock that mirrors the time on a connected 
             RTC module (e.g., DS1307). The clock can be periodically synchronized
             with the RTC, with customizable intervals to ensure accuracy.
             Optionally, it uses a mutex for safe concurrent access.
*/
/**********************************************************************/
class Time {
    #define STD_SYNC_INTERVAL_MSEC  3*60*60*1000    //!< Default synchronization interval (3 hours)
    #define MIN_SYNC_INTERVAL_MSEC  500             //!< Minimum allowable synchronization interval (500 ms)
    #define NO_SYNC                 0               //!< Value indicating no synchronization after initialization

    public:
        /**********************************************************************/
        /*!
            @brief   Default constructor for the Time class.
            @details Initializes all attributes, sets the start date to 
                     January 1, 2024, and establishes the default sync interval.
        */
        /**********************************************************************/
        Time(): _pBus(nullptr), _pMutex(&_dummyMutex), _validRTC(false), 
                _startTimer(millis()), _startDate(2024, 1, 1, 0, 0, 0),
                _syncInterval(STD_SYNC_INTERVAL_MSEC), _lastSync(_startTimer), _errorCounterInit(0) {}

        /**********************************************************************/
        /*!
            @brief   Destructor for the Time class.
            @details Deletes the mutex if it was created internally by this class.
        */
        /**********************************************************************/
        ~Time() = default;

        /**********************************************************************/
        /*!
            @brief   Initializes the Time object with an RTC module and optional synchronization settings.
            @param   now            The current system time in milliseconds.
            @param   pBus           Pointer to the TwoWire (I2C) bus connected to the RTC.
            @param   pMutex         Optional pointer to a mutex for safe access; if null, a mutex will be created.
            @param   syncInterval   Optional synchronization interval in milliseconds (default is 3 hours).
            @note    If `syncInterval` is provided but below the minimum allowed value, it will be adjusted.
                    If no mutex is provided, an internal mutex is created and managed by the class.
        */
        /**********************************************************************/
        void begin(uint32_t now, TwoWire *pBus, Mutex * pMutex = nullptr, uint32_t syncInterval = STD_SYNC_INTERVAL_MSEC) {
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
            if (_RTC.begin(_pBus)) {        // Attempt to start communication with the RTC
                _startDate = _RTC.now();    // Sync software time with RTC
                _startTimer = now;          // Set the reference start time
                _lastSync = now;            // Set the last sync time to current time
                _validRTC = true;           // Mark RTC as valid and synchronized
            } else {
                // If RTC fails, log an error and set a default date/time
                LOG(F("Could not sync software clock with RTC .. setting to 2024-12-24 11:11:11"));
                _startDate = DateTime(2024, 12, 24, 11, 11, 11);
                _validRTC = false;          // Mark RTC as invalid
                _errorCounterInit++;
            }
            _pMutex->free();                // Unlock the mutex after initialization
        }

        /**********************************************************************/
        /*!
            @brief   Updates the software clock based on system time and sync interval.
            @param   now  The current system time in milliseconds.
            @note    If the sync interval has elapsed, it synchronizes with the RTC.
                    Otherwise, it updates the software clock based on elapsed system time.
                    This method should be called regularly in the main loop.
        */
        /**********************************************************************/
        void loop(uint32_t now) {
            if ((now - _lastSync >= _syncInterval) && (_syncInterval != NO_SYNC)){
                // time for sync
                bool syncDone = false;
                if (_validRTC == false){
                        // no successfull init until now ==> try a new init
                        if (_pMutex->tryLock()){ 
                            if (_RTC.begin(_pBus)) {        // Attempt to start communication with the RTC
                                _startDate = _RTC.now();    // Sync software time with RTC
                                _startTimer = now;          // Set the reference start time
                                _lastSync = now;            // only in case of success set the last sync time to current time
                                _validRTC = true;           // Mark RTC as valid and synchronized                   
                                syncDone = true;
                            } else {
                                _errorCounterInit++;
                            }
                            _pMutex->free();
                        } 
                } else {
                    // read time from RTC and update/sync SW clock 
                    if (_pMutex->tryLock() == true){
                        _startDate = _RTC.now();    // Update the base date/time from RTC
                        _pMutex->free();
                        _startTimer = now;          // Reset the reference start time
                        _lastSync   = now;          // only in case of success record the last sync time
                        syncDone    = true;
                    }
                    // Attention: if MUTEX is haevily used by other applications a sync on regular base is not garantueed 

                }
                if (syncDone == false){
                    // update was not successfull this time 
                    // sync counter/timer are not reseted ==> sync will be tried next loop again
                    // do for now SW update only
                    _loopDate = _startDate + TimeSpan((now - _startTimer) / 1000);
                }
            } else {
                // only SW clock update .. no HW access
                _loopDate = _startDate + TimeSpan((now - _startTimer) / 1000);
            }
        }

        /**********************************************************************/
        /*!
            @brief   Returns the last updated software clock time.
            @return  The last updated time as a DateTime object.
            @note    This is the time calculated by the loop() function based on 
                     the last synchronization or system time adjustments.
        */
        /**********************************************************************/
        DateTime getLoopDateTime() const { return _loopDate; }

        /**********************************************************************/
        /*!
            @brief   returns count of failed init tries
            @return  current retry counter for init
        */
        /**********************************************************************/
        uint32_t getErrorCounterInit()  const { return _errorCounterInit; }

    protected:
        uint32_t        _startTimer;     /*!< System time (in milliseconds) at last RTC synchronization */
        uint32_t        _syncInterval;   /*!< Interval between RTC and software clock syncs (in ms) */
        uint32_t        _lastSync;       /*!< System time (in milliseconds) of last RTC synchronization */
        DateTime        _startDate;      /*!< Last synced DateTime from the RTC */
        DateTime        _loopDate;       /*!< Current DateTime as calculated by the software clock */

        TwoWire *       _pBus;           /*!< Pointer to the I2C bus for RTC communication */
        Mutex  *        _pMutex;         /*!< Pointer to mutex for safe access to shared resources */
        Mutex           _dummyMutex;     /*!< dummy Mutex if no Mutex is given */
        bool            _validRTC;       /*!< Flag indicating if the RTC is valid and synchronized */

        RTC_DS1307      _RTC;            /*!< RTC module instance for time synchronization */

        uint32_t        _errorCounterInit; /*! RTC init error counter */
};
