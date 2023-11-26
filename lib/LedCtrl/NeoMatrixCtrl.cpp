#include "NeoMatrixCtrl.hpp"
#include "Debug.hpp"
#include "NeoMatrixAni.hpp"


NeoMatrixCtrl::NeoMatrixCtrl(Adafruit_NeoMatrix * pMatrix) : Ctrl()
{
    ASSERT(pMatrix != NULL,"pMatrix must not be NULL");
    _pMatrix = pMatrix;
    _pMatrix->begin();
    // does not work to dim colors down and then UP (original color is not stored)
    // the implement dimming by color recalculation
    pMatrix->setBrightness(255);  
    pMatrix->fillScreen(pMatrix->Color(0,0,0));
    pMatrix->show();


    addAni(new MatrixOffAni());
    addAni(new MatrixStaticAni());
    addAni(new MatrixRainbowAni());
    addAni(new MatrixBreathAni());
    addAni(new MatrixMultiFlashAni());
    addAni(new MatrixBoxAni(false));
    addAni(new MatrixBoxAni(true));
    addAni(new MatrixGifFileAni());

    // select first
    setup(0);

}

NeoMatrixCtrl::~NeoMatrixCtrl()
{

}


void NeoMatrixCtrl::loop(u32_t time){
    if (_pCurrentAni == NULL)           return;
    if (_pMatrix == NULL)               return;
    if (_mutexSetup.isLocked()==true)   return;  // do not wait 

    _mutexSetup.lock();
    ((NeoMatrixAni*)_pCurrentAni)->loop(time,_pMatrix);
    _mutexSetup.unlock();
}


/*
const uint16_t colors[] = {
  matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };


int x    = mw;
int pass = 0;

void loop() {
  matrix->fillScreen(0);
  matrix->setCursor(x, 0);
  matrix->print(F("Howdy"));
  if(--x < -36) {
    x = matrix->width();
    if(++pass >= 3) pass = 0;
    matrix->setTextColor(colors[pass]);
  }
  matrix->show();
  delay(100);
}

*/
