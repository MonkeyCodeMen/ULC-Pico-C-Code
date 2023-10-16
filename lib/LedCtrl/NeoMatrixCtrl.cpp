#include "NeoMatrixCtrl.hpp"
#include "Debug.hpp"
#include "NeoMatrixAni.hpp"


NeoMatrixCtrl::NeoMatrixCtrl(int pin,int dimX,int dimY,int countX,int countY,int order,int type) : Ctrl()
{
    _pMatrix = new Adafruit_NeoMatrix(dimX,dimY,countX,countY,pin,order,type);
    ASSERT(_pMatrix != NULL,"could not create matrix object");
    _pMatrix->begin();

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


void NeoMatrixCtrl::loop(){
    u32_t time=millis();
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
