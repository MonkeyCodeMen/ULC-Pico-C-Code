#pragma once

#include <MainConfig.h>
#ifdef WITH_DISPLAY
    #include <SPI.h>
    #include <TFT_eSPI.h>         // Hardware-specific library
    #define TFT_DIM     int(255*1.0)
    class Display:public TFT_eSPI{

        public:
            Display():TFT_eSPI() {}
            ~Display() = default;
            void begin();
    };

    extern Display display;


#endif
