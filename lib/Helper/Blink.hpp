#pragma once
#include <Arduino.h>
#include <vector>
#include <Debug.hpp>

/**********************************************************************/
/*!
  @class   BlinkingLED
  @brief   Manages the blinking behavior of an LED on a specified pin.
  @details Allows the LED to be controlled in either a static on/off mode or 
           using a custom blink sequence. The blink sequence can be set with 
           a vector of on/off durations, and the LED state is updated based 
           on this sequence.
           the sequence start in normal cases with ON 
           the sequence is limited to 250 on/off pairs = 125 on phases

  @example pin assignement is been configured on construction
           BlinkingLED  blink = BlinkingLED(LED_BUILTIN);

           at any time the sequence can be suspended by setting the LED to static value
           blink.on();
           blink.off();

           but the class is been designed to follow a configurable blink sequence 
           so first setup the sequence:
           blink.setup(const std::vector<uint32_t> {125, 500, 125, 500, 125, 2000} );

           this example results in a seqeunce:
           125mSec on
           500mSec off
           125mSec on
           500mSec off
           125mSec on
           2000msec off
           ... repeat

           if you want to start with an off phase set the invers flag in the constructor invers to reality

           now call the loop function of the object in your loop function

           void loop{
            uint32_t now = millis();
            blink.loop(now);

            ... rest of code
           }



*/
/**********************************************************************/
#define BLINK_DEFAULT_LAST_OFF      1000 /*<! default off time in mSec for last off phase, used if sequence of odd length is provided*/
#define BLINK_MAX_SEQUENCE_LENGTH   250
class BlinkingLED {
  public:
    /**********************************************************************/
    /*!
      @brief   Constructor to initialize the LED pin and set it as OUTPUT.
      @param   pin    The digital pin number to which the LED is connected.
      @param   invers true:  ON=LOW / OFF=HIGH    false: ON=HIGH / OFF=LOW 
                      can be used to start with OFF LED as sequence order too
      @note    Sets default values for internal variables and initializes the pin.
    */
    /**********************************************************************/
    BlinkingLED(int pin,bool invers=true) : 
      _pin(pin), _currentStep(0), _lastTime(0),
      _onState(invers?true,LOW:HIGH), _offState(invers?true,HIGH:LOW),
      _currentState(_offState),_static(true)
    {
        // init pin
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin,_currentState);
    }

    /////////////////////////////////////////////////////////////
    // control functions

    /**********************************************************************/
    /*!
      @brief   Turns the LED on and sets it to static mode.
      @note    Disables any blinking sequence and keeps the LED permanently on.
    */
    /**********************************************************************/
    void on() {
        _static = true;
        _currentState = _onState;
        digitalWrite(_pin, _currentState);
    }

    /**********************************************************************/
    /*!
      @brief   Turns the LED off and sets it to static mode.
      @note    Disables any blinking sequence and keeps the LED permanently off.
    */
    /**********************************************************************/
    void off() {
        _static = true;
        _currentState = _offState;
        digitalWrite(_pin, _currentState);
    }

    /////////////////////////////////////////////////////////////
    // sequence setup function

    /**********************************************************************/
    /*!
      @brief   Sets up the blink sequence for the LED.
      @param   sequence  A vector of integers specifying on/off durations in milliseconds (limited to 250 because of sequence counter)
      @note    Resets the sequence to the beginning and exits static mode to enable blinking.
               for a full sequence we need an even count of states {on-time,off-time,on-time,off-time}
               if the provided count is odd an aditional phase of BLINK_DEFAULT_LAST_OFF will be added
    */
    /**********************************************************************/
    void setup(const std::vector<uint32_t>& sequence) {
        
        // first check sequence
        uint32_t count = sequence.size();
        if ((count > BLINK_MAX_SEQUENCE_LENGTH) || (count==0 )){
          LOG(F("invalid sequence length ==> LED off"));
          _static = true;
          _sequence.clear();
          _currentStep=0;
          _currentState = _offState;
          digitalWrite(_pin, _currentState);          
          return;
        } else {
          _sequence = sequence;   // copy sequence to this object (the source may be changed)
          // correct sequence length if necessary
          if (count % 2 == 1){
            LOG(F("default last off time added to sequence vector"));
            _sequence.push_back(BLINK_DEFAULT_LAST_OFF);
          }
        }
        
        // a high delay between setup and next loop will cause an inaccurate first on phase
        _lastTime = millis();   // Reset the timer

        // begin always with LED on
        _currentStep = 0;                   // Reset the sequence to the beginning
        _currentState = _onState;
        digitalWrite(_pin, _currentState);  // Set the LED state

        _static = false;
    }

    /////////////////////////////////////////////////////////////
    // update function

    /**********************************************************************/
    /*!
      @brief   Updates the LED state based on the current blink sequence.
      @param   now  The current time in milliseconds.
      @note    This function should be called regularly in the main loop to maintain the blinking sequence.
               If no sequence is set or the LED is in static mode, the function exits immediately.
    */
    /**********************************************************************/
    void loop(uint32_t now) {
        if (_static == true) {
            return; // No sequence is set or LED is in static mode
        }

        // Check if the current step duration has passed
        if (now - _lastTime >= _sequence[_currentStep]) {
            // Toggle the LED state (on if even step, off if odd step)
            _currentState = (_currentStep % 2 == 0) ? _onState : _offState;
            digitalWrite(_pin, _currentState);  // Set the LED state

            // Move to the next step, loop back to the start if at the end
            _currentStep = (_currentStep + 1) % _sequence.size();
            _lastTime = now;  // Record the time for this step
        }
    }

private:
    int _pin;                    /*!< Pin to which the LED is connected                     */
    std::vector<uint32_t> _sequence; /*!< Vector of blink sequence durations in milliseconds   */
    uint8_t _currentStep;        /*!< Current step in the sequence                          */
    uint32_t _lastTime;          /*!< Time when the last step started                       */
    PinStatus _onState;          /*!< PIN status for LED ON                                 */
    PinStatus _offState;         /*!< PIN status for LED OFF                                */
    PinStatus _currentState;     /*!< Current state of the LED (HIGH or LOW)                */
    bool _static;                /*!< Flag indicating if the LED is in static mode (on/off) */
};

