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


    _addAni(new MatrixOffAni());
    _addAni(new MatrixStaticAni());
    _addAni(new MatrixBreathAni());
    _addAni(new MatrixRainbowFlashAni());
    _addAni(new MatrixMultiFlashAni());

    // select first
    setup(0);

}

NeoMatrixCtrl::~NeoMatrixCtrl()
{
    if (_pMatrix != NULL)   delete(_pMatrix);
}


void NeoMatrixCtrl::loop(u32_t time){
    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }
    _mutexSetup.lock();
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pMatrix != NULL,"");
 
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
