
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


#pragma once

#include <Arduino.h>
#include <MainConfig.h>
#include <Wire.h>
#include <Mutex.hpp>
#include <Debug.hpp>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_BusIO_Register.h>
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
class BufferdClock {
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
        BufferdClock(): _pBus(nullptr), _pMutex(&_dummyMutex), _validRTC(false), 
                _startDate(2024, 1, 1, 0, 0, 0),
                _syncInterval(STD_SYNC_INTERVAL_MSEC), _lastSync(millis()), _errorCounterInit(0) {}

        /**********************************************************************/
        /*!
            @brief   Destructor for the Time class.
            @details Deletes the mutex if it was created internally by this class.
        */
        /**********************************************************************/
        ~BufferdClock() = default;

        /**********************************************************************/
        /*!
            @brief   Initializes the Time object with an RTC module and optional synchronization settings.
            @param   pBus           Pointer to the TwoWire (I2C) bus connected to the RTC.
            @param   pMutex         Optional pointer to a mutex for safe access; if null, a mutex will be created.
            @param   syncInterval   Optional synchronization interval in milliseconds (default is 3 hours).
            @note    If `syncInterval` is provided but below the minimum allowed value, it will be adjusted.
                    If no mutex is provided, an internal mutex is created and managed by the class.
        */
        /**********************************************************************/
        void begin(TwoWire *pBus, Mutex * pMutex = nullptr, uint32_t syncInterval = STD_SYNC_INTERVAL_MSEC);

        /**********************************************************************/
        /*!
            @brief   Updates the software clock based on system time and sync interval.
            @param   now  The current system time in milliseconds.
            @note    If the sync interval has elapsed, it synchronizes with the RTC.
                    Otherwise, it updates the software clock based on elapsed system time.
                    This method should be called regularly in the main loop.
        */
        /**********************************************************************/
        void loop(uint32_t now);

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
            @brief   update the time of this object and the underlieing RTC 
            @param   newDateTime   date/time for update
            @param   blocking      set this flag to false if method should not wait if Mutex is not available
            @return  true : success   false: failed
        */
        /**********************************************************************/
        bool udateRTC(DateTime newDateTime,bool blocking=true);

        /**********************************************************************/
        /*!
            @brief   returns count of failed init tries
            @return  current retry counter for init
        */
        /**********************************************************************/
        uint32_t getErrorCounterInit()  const { return _errorCounterInit; }

    protected:
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


extern BufferdClock bufferedClock;
