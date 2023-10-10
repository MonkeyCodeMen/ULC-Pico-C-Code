#include "ComDispatch.hpp"
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
            LOG("unknown module .. could not disptach Frame");
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
            LOG("unknown index for Led module  .. skip frame");
            return;
    }
    if (pFrame->withPar == true){
        // this module accept only u32_t parameter
        u32_t p1 = convertStrToInt(pFrame->par1.c_str());
        u32_t p2 = convertStrToInt(pFrame->par2.c_str());
        u32_t p3 = convertStrToInt(pFrame->par3.c_str());
        u32_t p4 = convertStrToInt(pFrame->par4.c_str());
        
        pLedCtrl->setup(pFrame->command);
        pLedCtrl->setup(p1,p2,p3,p4,pFrame->length,pFrame->pData);
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
            LOG("unknown index for Led module  .. skip frame");
            return;
    }

    if (pFrame->withPar == true){
        // this module accept only u32_t parameter
        u32_t p1 = convertStrToInt(pFrame->par1.c_str());
        u32_t p2 = convertStrToInt(pFrame->par2.c_str());
        u32_t p3 = convertStrToInt(pFrame->par3.c_str());
        u32_t p4 = convertStrToInt(pFrame->par4.c_str());
        
        pRgbCtrl->setup(pFrame->command);
        pRgbCtrl->setup(p1,p2,p3,p4,pFrame->length,pFrame->pData);
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
            LOG("unknown index for Neo Stripe module  .. skip frame");
            return;
    }
    if (pFrame->withPar == true){
        // this module accept only u32_t parameter
        u32_t p1 = convertStrToInt(pFrame->par1.c_str());
        u32_t p2 = convertStrToInt(pFrame->par2.c_str());
        u32_t p3 = convertStrToInt(pFrame->par3.c_str());
        u32_t p4 = convertStrToInt(pFrame->par4.c_str());
        
        pStripeCtrl->setup(pFrame->command);
        pStripeCtrl->setup(p1,p2,p3,p4,pFrame->length,pFrame->pData);
    } else {
        pStripeCtrl->setup(pFrame->command);  // use default parameter for 
    }

}

void ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame)
{

}
