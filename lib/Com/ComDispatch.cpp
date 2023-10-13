#include "ComDispatch.hpp"
#include "ExternObjects.hpp"
#include "Debug.hpp"
#include "helper.hpp"


ComDispatch::ComDispatch()
{
}

void ComDispatch::dispatchFrame(ComFrame * pFrame)
{
    switch(pFrame->module){
        case('C'):  dispatchCommonFrame(pFrame);    break;
        case('L'):  dispatchLedFrame(pFrame);       break;
        case('R'):  dispatchRgbLedFrame(pFrame);    break;
        case('S'):  dispatchNeoStripeFrame(pFrame); break;
        case('M'):  dispatchNeoMatrixFrame(pFrame); break;
        
        default:
            LOG(F("unknown module .. could not disptach Frame"));
    }
}

void ComDispatch::dispatchCommonFrame(ComFrame * pFrame){

}


void ComDispatch::dispatchLedFrame(ComFrame * pFrame)
{
    LedCtrl * pLedCtrl;
    switch(pFrame->index){
        case 0: pLedCtrl = pLedCtrl1; break;
        case 1: pLedCtrl = pLedCtrl2; break;
        default:
            LOG(F("unknown index for Led module  .. skip frame"));
            return;
    }
    if (pFrame->withPar == true){
        pLedCtrl->setup(pFrame->command);
        pLedCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->length,pFrame->pData);
    } else {
        pLedCtrl->setup(pFrame->command);  // use default parameter for 
    }
}

void ComDispatch::dispatchRgbLedFrame(ComFrame * pFrame)
{
    RgbLedCtrl * pRgbCtrl;
    switch(pFrame->index){
        case 0: pRgbCtrl = pRgbCtrl1; break;
        default:
            LOG(F("unknown index for Led module  .. skip frame"));
            return;
    }

    if (pFrame->withPar == true){
        pRgbCtrl->setup(pFrame->command);
        pRgbCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->length,pFrame->pData);
    } else {
        pRgbCtrl->setup(pFrame->command);  // use default parameter for 
    }
}

void ComDispatch::dispatchNeoStripeFrame(ComFrame * pFrame)
{
    NeoStripeCtrl * pStripeCtrl;
    switch(pFrame->index){
        case 0: pStripeCtrl = pNeoStripeCtrl1; break;
        case 1: pStripeCtrl = pNeoStripeCtrl2; break;
        default:
            LOG(F("unknown index for Neo Stripe module  .. skip frame"));
            return;
    }
    if (pFrame->withPar == true){
        pStripeCtrl->setup(pFrame->command);
        pStripeCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->length,pFrame->pData);
    } else {
        pStripeCtrl->setup(pFrame->command);  // use default parameter for 
    }

}

void ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame)
{
    NeoMatrixCtrl * pMatrixCtrl;
    switch(pFrame->index){
        case 0: pMatrixCtrl = pNeoMatrixCtrl1; break;
        case 1: pMatrixCtrl = pNeoMatrixCtrl2; break;
        default:
            LOG(F("unknown index for Neo matrix module  .. skip frame"));
            return;
    }
    if (pFrame->withPar == true){
        pMatrixCtrl->setup(pFrame->command);
        pMatrixCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->length,pFrame->pData);
    } else {
        pMatrixCtrl->setup(pFrame->command);  // use default parameter for 
    }

}
