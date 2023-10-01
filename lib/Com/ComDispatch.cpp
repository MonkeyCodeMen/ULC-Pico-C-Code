#include "ComDispatch.hpp"
#include "../Debug/Debug.hpp"


ComDispatch::ComDispatch()
{
}

void ComDispatch::dispatchFrame(ComFrame * pFrame)
{
    switch(pFrame->module){
        case('L'):  dispatchLedFrame(pFrame);       break;
        case('R'):  dispatchRgbLedFrame(pFrame);    break;
        case('S'):  dispatchNeoStripeFrame(pFrame); break;
        case('M'):  dispatchNeoMatrixFrame(pFrame); break;
        
        default:
            LOG("unknown module .. could not disptach Frame");
    }
}


void ComDispatch::dispatchLedFrame(ComFrame * pFrame)
{
    LedCtrl * pLedCtrl;
    switch(pFrame->index){
        case 0: pLedCtrl = pLedCtrl1; break;
        case 1: pLedCtrl = pLedCtrl2; break;
        default:
            LOG("unknown index for Led module  .. skip frame");
            return;
    }
    // this module accept only u32_t parameter
    u32_t p1 = pFrame->par1.toInt();
    u32_t p2 = pFrame->par2.toInt();
    u32_t p3 = pFrame->par3.toInt();
    u32_t p4 = pFrame->par4.toInt();
    
    pLedCtrl->setup(pFrame->command,p1,p2,p3,p4,pFrame->length,pFrame->pData);
}

void ComDispatch::dispatchRgbLedFrame(ComFrame * pFrame)
{
    RgbLedCtrl * pRgbCtrl;
    switch(pFrame->index){
        case 0: pRgbCtrl = pRgbCtrl1; break;
        default:
            LOG("unknown index for Led module  .. skip frame");
            return;
    }
    // this module accept only u32_t parameter
    u32_t p1 = pFrame->par1.toInt();
    u32_t p2 = pFrame->par2.toInt();
    u32_t p3 = pFrame->par3.toInt();
    u32_t p4 = pFrame->par4.toInt();
    
    pRgbCtrl->setup(pFrame->command,p1,p2,p3,p4,pFrame->length,pFrame->pData);
}

void ComDispatch::dispatchNeoStripeFrame(ComFrame * pFrame)
{

}

void ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame)
{

}
