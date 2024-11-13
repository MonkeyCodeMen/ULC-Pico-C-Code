#pragma once

#include <NeoMatrixGif.hpp>

/*
    class to implement an advents calendar based on NeoMatrixGif

    for each day (1-24) there is a subdirectory "./advent/01/" to ./advent/24/" holding the GIF's animation for the day
    there is common directory "./advent/x/" holding additional animatoins that will be mixed with the dailies one on a rare base

    this class now take the current date in a loop, select the coresponding animation directory and shows the content (various if animatoins for the day on the screen)


*/

class MatrixGifCalendarAni : public NeoMatrixGifFileAni{
}