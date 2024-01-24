#pragma once

#include <MainConfig.h>

#ifdef WITH_SD_CARD
    #include <SPI.h>
    #include <SD.h>
    extern SDClass globalSDcard0;
#endif 
