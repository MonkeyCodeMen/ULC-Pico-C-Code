#pragma once
#include <Arduino.h>
#include <vector>

/**********************************************************************/
/*!
  @class   BlinkingLED
  @brief   Manages the blinking behavior of an LED on a specified pin.
  @details Allows the LED to be controlled in either a static on/off mode or 
           using a custom blink sequence. The blink sequence can be set with 
           a vector of on/off durations, and the LED state is updated based 
           on this sequence.
*/
/**********************************************************************/
class BlinkingLED {
public:
    /**********************************************************************/
    /*!
      @brief   Constructor to initialize the LED pin and set it as OUTPUT.
      @param   pin  The digital pin number to which the LED is connected.
      @note    Sets default values for internal variables and initializes the pin.
    */
    /**********************************************************************/
    BlinkingLED(int pin) {
        _pin = pin;
        pinMode(_pin, OUTPUT);
        _currentStep = 0;
        _lastTime = 0;
        _ledState = LOW;
        _static = true;
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
        _ledState = HIGH;
        digitalWrite(_pin, HIGH);
    }

    /**********************************************************************/
    /*!
      @brief   Turns the LED off and sets it to static mode.
      @note    Disables any blinking sequence and keeps the LED permanently off.
    */
    /**********************************************************************/
    void off() {
        _static = true;
        _ledState = LOW;
        digitalWrite(_pin, LOW);
    }

    /////////////////////////////////////////////////////////////
    // sequence setup function

    /**********************************************************************/
    /*!
      @brief   Sets up the blink sequence for the LED.
      @param   sequence  A vector of integers specifying on/off durations in milliseconds.
      @note    Resets the sequence to the beginning and exits static mode to enable blinking.
    */
    /**********************************************************************/
    void setup(const std::vector<uint32_t>& sequence) {
        _sequence = sequence;
        _currentStep = 0;      // Reset the sequence to the beginning
        _lastTime = millis();  // Reset the timer
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
        if (_sequence.empty() || _static == true) {
            return; // No sequence is set or LED is in static mode
        }

        // Check if the current step duration has passed
        if (now - _lastTime >= _sequence[_currentStep]) {
            // Toggle the LED state (on if even step, off if odd step)
            _ledState = (_currentStep % 2 == 0) ? LOW : HIGH;
            digitalWrite(_pin, _ledState);  // Set the LED state

            // Move to the next step, loop back to the start if at the end
            _currentStep = (_currentStep + 1) % _sequence.size();
            _lastTime = now;  // Record the time for this step
        }
    }

private:
    int _pin;                    /*!< Pin to which the LED is connected                     */
    std::vector<uint32_t> _sequence; /*!< Vector of blink sequence durations in milliseconds   */
    int _currentStep;            /*!< Current step in the sequence                          */
    uint32_t _lastTime;          /*!< Time when the last step started                       */
    int _ledState;               /*!< Current state of the LED (HIGH or LOW)                */
    bool _static;                /*!< Flag indicating if the LED is in static mode (on/off) */
};

