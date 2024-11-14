#pragma once
#include <Arduino.h>
#include <Mutex.hpp>

extern Mutex I2C0_mutex;
#define I2C_100KHZ  100*1000
#define I2C_400KHZ  400*1000
#define I2C_DEFAULT_SPEED   I2C_400KHZ


extern Mutex I2C1_mutex;
extern Mutex SPI0_mutex;
extern Mutex SPI1_mutex;
