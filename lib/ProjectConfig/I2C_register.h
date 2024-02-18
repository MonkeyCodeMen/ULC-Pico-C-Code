#pragma once

#include <Arduino.h>


#define I2C_ADR_SLAVE     0x58
#define PWM_CHANNELS 12
#define CHAR_ARRAY_LENGTH 5
/////////////////////////////////////////////////////
// slave receive register Bank
struct I2C_SlaveReceiveReg_Struct{
    uint8_t PWM_soll[PWM_CHANNELS];
};

/////////////////////////////////////////////////////
// slave transmit register Bank
struct I2C_SlaveTransmitReg_Struct{
    uint8_t PWM_ist[PWM_CHANNELS];
};
extern I2C_SlaveReceiveReg_Struct  I2C_slaveSoll;
extern I2C_SlaveTransmitReg_Struct I2C_slaveIst;

