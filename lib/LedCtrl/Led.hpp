#ifndef LED_H
#define LED_H

#include <Arduino.h>

#define LED_MAX 255
#define LED_OFF 0
#define LED_MIN 0


#define LED_LOGIC_INVERS    false
#define LED_PWM_RANGE       255
#define LED_DIM_ACCURACY    LED_PWM_RANGE


class Led
{
    public:
        Led(int pin);
        ~Led() = default;

        void set(u8_t value);

    private:
        int     _pin;
};


#endif /* LED_H*/