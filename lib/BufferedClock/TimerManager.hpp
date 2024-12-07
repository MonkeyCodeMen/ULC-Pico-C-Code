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
#include <Time.hpp>
#include <TimeSpanExt.hpp>
#include <vector>
#include <algorithm>
#include <Split.hpp>
#include <helper.h>

struct TimerDef{
    // start Time
    uint8_t     start_hour; 
    uint8_t     start_minute;
    // duration
    uint16_t     duration_minute;

    // init constructor
    TimerDef()                              : start_hour(0),start_minute(0),duration_minute(0) {}
    TimerDef(uint8_t h,uint8_t m,uint8_t d) : start_hour(h),start_minute(m),duration_minute(d) {}

    // convert & return
    Time        startTime()             {return Time(start_hour,start_minute,0);                }
    TimeSpan    startTimeSpan()         {return TimeSpan(0,start_hour,start_minute,0);          }   
    TimeSpanExt startTimeSpanExt()      {return TimeSpanExt(0,start_hour,start_minute,0);       }
    Time        durationTime()          {return Time(duration_minute       *SECONDS_PER_MINUTE);} 
    TimeSpan    durationTimeSPan()      {return TimeSpan(duration_minute   *SECONDS_PER_MINUTE);} 
    TimeSpanExt durationTimeSpanExt()   {return TimeSpanExt(duration_minute*SECONDS_PER_MINUTE);} 
};

#define TIMER_MANAGER_OK                0
#define TIMER_MANAGER_INVALID_CONFIG    1
#define TIMER_MANAGER_PAST_TIME         DateTime(2020,12,24,10,10,10)
/*
    TimerManager manager("8:30-120,18:45-180");

    std::vector<TimerDef> timerList = {
        TimerDef(8, 30, 120), // Start at 8:30, duration 120 minutes
        TimerDef(14, 45, 90), // Start at 14:45, duration 90 minutes
        TimerDef(18, 0, 60)   // Start at 18:00, duration 60 minutes
    };
    TimerManager manager(timerList);

    TimerMananger timer;

    timer.newEventList(8:30-120,18:45-10");
    timer.add("22:30-45")

*/
/**
 * @class TimerManager
 * @brief A class to manage timer events with support for multiple configurations and thread-safe initialization.
 */
class TimerManager {
public:
    /**
     * @brief Default constructor. Initializes the manager in a thread-safe initialization state.
     */
    TimerManager() : _init(true),_state(SWITCHED_OFF),_lastCheckedTime(TIMER_MANAGER_PAST_TIME) {}

    /**
     * @brief Constructor that initializes the manager with a list of timer events from a string.
     * @param list A string representing timer events in the format "HH:MM-DURATION,HH:MM-DURATION,...".
     */
    TimerManager(String& list) : _init(true),_state(SWITCHED_OFF),_lastCheckedTime(TIMER_MANAGER_PAST_TIME) {
        _parseTimerEventList(list);
        _init = false; // Initialization complete
    }
    TimerManager(char * list) :_init(true),_state(SWITCHED_OFF),_lastCheckedTime(TIMER_MANAGER_PAST_TIME){
        String strList(list);
        _parseTimerEventList(strList);
        _init = false; // Initialization complete
    } 

    /**
     * @brief Constructor that initializes the manager with a vector of TimerDef objects.
     * @param timerList A vector of TimerDef objects to initialize the timer manager.
     */
    TimerManager(std::vector<TimerDef>& timerList)
        : _init(true),_state(SWITCHED_OFF),_lastCheckedTime(TIMER_MANAGER_PAST_TIME), _timerList(timerList) {
        _init = false; // Initialization complete
    }

    /**
     * @brief Copy constructor.
     * @param other The TimerManager instance to copy from.
     */
    TimerManager(TimerManager& other)
        : _init(true), _timerList(other._timerList), _state(other._state), 
        _lastCheckedTime(other._lastCheckedTime) {
            _init=other._init;
        }

    /**
     * @brief Destructor. Clears the timer list and resets the initialization state.
     */
    ~TimerManager() {
        _init = true; // Reset to default state
        _state = SWITCHED_OFF;
        _timerList.clear();
        _lastCheckedTime = TIMER_MANAGER_PAST_TIME;
    }

    /**
     * @brief Checks if the timer manager is currently switched on.
     * @return True if switched on, false otherwise.
     */
    bool switchedOn() const { return _state == SWITCHED_ON; }

    /**
     * @brief Checks if the timer manager is currently switched off.
     * @return True if switched off, false otherwise.
     */
    bool switchedOff() const { return _state == SWITCHED_OFF; }

    /**
     * @brief Retrieves the current list of TimerDef objects.
     * @return A const reference to the vector of TimerDef objects.
     */
    const std::vector<TimerDef>& getTimerList() const { return _timerList; }
    int size() {return _timerList.size(); }

    /**
     * @brief Processes timer events. Should be called in a loop.
     * Exits immediately if the manager is in an initialization state.
     * (full) check will be performed every minute only (=resolution of timer)
     */
    void loop(DateTime dateTime) {
        TimerState res=SWITCHED_OFF;    // default off but do not reload now to avoid jitter
        if (_init == true || _timerList.empty()) {
            _state = res;
            return;
        }

        Time currentTime(dateTime);
        // ceck if we have another time, ignore seconds
        currentTime.setSec(0);
        if (_lastCheckedTime == currentTime) {
            return;
        }
        _lastCheckedTime = currentTime; // Cache aktualisieren
        bool activeTimer = false;

        for (int i = 0; i < _timerList.size(); i++) {
            TimerDef entry = _timerList[i];
            Time startTime = entry.startTime();
            Time endTime = startTime + entry.durationTime();

            if (currentTime >= startTime && currentTime <= endTime) {
                res = SWITCHED_ON;
                activeTimer = true;
                break;
            } else if (currentTime < startTime) {
                res = SWITCHED_OFF;
                activeTimer = false;
                break;
            }
        }
        _state = res;       // now the result is stable ==> take over
    }

    /**
     * @brief Loads a new list of timer events from a string.
     * @param list A string representing timer events in the format "HH:MM-DURATION,HH:MM-DURATION,...".
     * @return A status code indicating success or failure.
     */
    int newEventList(String list) {
        _init = true;
        _timerList.clear();
        int result = _parseTimerEventList(list);
        _init = false;
        return result;
    }

    /**
     * @brief Adds a list of timer events from a string.
     * @param list A string representing timer events in the format "HH:MM-DURATION,HH:MM-DURATION,...".
     * @return A status code indicating success or failure.
     */
    int addEventList(String list) {
        _init = true;
        _state = SWITCHED_OFF;
        int result = _parseTimerEventList(list);
        _init = false;
        return result;
    }

    /**
     * @brief Loads a new list of timer events from a vector of TimerDef objects.
     * @param timerList A vector of TimerDef objects to initialize the timer manager.
     */
    void newEventList(std::vector<TimerDef>& timerList) {
        _init = true;
        _state = SWITCHED_OFF;
        _timerList = timerList;
        _sortList();
        _init = false;
    }

    /**
     * @brief Adds more timer events to the list from a vector of TimerDef objects.
     * @param timerList A vector of TimerDef objects to add to the timer manager.
     */
    void addEventList(std::vector<TimerDef>& timerList) {
        _init = true;
        _state = SWITCHED_OFF;
        _timerList.insert(_timerList.end(), timerList.begin(), timerList.end());
        _sortList();
        _init = false;
    }

    /**
     * @brief Adds a single timer event from a string.
     * @param eventString A string representing a single timer event in the format "HH:MM-DURATION".
     */
    void addEvent(String eventString) {
        _init = true;
        _state = SWITCHED_OFF;
        Split entry(eventString, '-');
        String startStr = entry.getNextListEntry();
        Split startList(startStr, ':');

        uint8_t start_hour = convertStrToInt(startList.getNextListEntry());
        uint8_t start_minute = convertStrToInt(startList.getNextListEntry());
        uint16_t duration = convertStrToInt(entry.getNextListEntry());

        if ((duration == 0) || (start_hour > 23) || (start_minute > 59)) {
            _init = false;
            return;
        }

        _timerList.emplace_back(start_hour, start_minute, duration);
        _sortList();
        _init = false;
    }

    /**
     * @brief Adds a single timer event from a TimerDef object.
     * @param event A TimerDef object to add to the timer manager.
     */
    void addEvent(TimerDef event) {
        _init = true;
        _timerList.emplace_back(event);
        _sortList();
        _init = false;
    }

    /**
     * @brief Adds a single timer event from individual parameters.
     * @param start_hour The start hour of the timer event (0-23).
     * @param start_minute The start minute of the timer event (0-59).
     * @param duration_minutes The duration of the timer event in minutes.
     */
    void addEvent(uint8_t start_hour, uint8_t start_minute, uint16_t duration_minutes) {
        if ((duration_minutes == 0) || (start_hour > 23) || (start_minute > 59)) {
            return;
        }

        _init = true;
        _timerList.emplace_back(start_hour, start_minute, duration_minutes);
        _sortList();
        _init = false;
    }

private:
    enum TimerState { SWITCHED_ON, SWITCHED_OFF };              //!< Timer states
    std::vector<TimerDef> _timerList;                           //!< List of TimerDef objects managed by the TimerManager
    TimerState _state = SWITCHED_OFF;                           //!< Current state of the timer manager
    bool _init;                                                 //!< Indicates if the TimerManager is in an initialization state
    Time _lastCheckedTime;                                      //!< time cache to check timer evenets only every minute

    /**
     * @brief Sorts the timer list based on start time.
     */
    void _sortList() {
        std::sort(_timerList.begin(), _timerList.end(),
                  [](TimerDef& a, TimerDef& b) { return a.startTime() < b.startTime(); });
    }

    /**
     * @brief Parses a string and initializes the timer event list.
     * @param list A string representing timer events in the format "HH:MM-DURATION,HH:MM-DURATION,...".
     * @return A status code indicating success or failure.
     */
    int _parseTimerEventList(String& list) {
        Split timerInputList(list, ',');
        while (timerInputList.isEndReached() == false) {
            String entryStr = timerInputList.getNextListEntry();
            Split entry(entryStr, '-');
            String startStr = entry.getNextListEntry();
            Split startPar(startStr, ':');

            uint8_t start_hour = convertStrToInt(startPar.getNextListEntry());
            uint8_t start_minute = convertStrToInt(startPar.getNextListEntry());
            uint16_t duration = convertStrToInt(entry.getNextListEntry());

            if ((duration == 0) || (start_hour > 23) || (start_minute > 59)) {
                return TIMER_MANAGER_INVALID_CONFIG;
            }

            _timerList.emplace_back(start_hour, start_minute, duration);
        }
        _sortList();
        return TIMER_MANAGER_OK;
    }
};
