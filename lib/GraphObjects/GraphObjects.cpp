#include <MainConfig.h>

#ifdef WITH_DISPLAY
    #include <SPI.h>
    #include <TFT_eSPI.h> // Hardware-specific library
    extern TFT_eSPI * pTFT;       // Invoke custom library

    #include <GraphObjects2D.hpp>
    #include <GraphObjects3D.hpp>
    #include <View3D.hpp>
    #include <cube.hpp>

    Cube cube(pTFT);
#endif
