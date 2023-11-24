#include "ComDispatch.hpp"
#include "ExternObjects.hpp"
#include "Debug.hpp"
#include "helper.hpp"


ComDispatch::ComDispatch()
{
}

bool ComDispatch::dispatchFrame(ComFrame * pFrame)
{
    bool res;
    switch(pFrame->module){
        case('C'):  res = dispatchCommonFrame(pFrame);    break;
        case('L'):  res = dispatchLedFrame(pFrame);       break;
        case('R'):  res = dispatchRgbLedFrame(pFrame);    break;
        case('S'):  res = dispatchNeoStripeFrame(pFrame); break;
        case('M'):  res = dispatchNeoMatrixFrame(pFrame); break;
        
        default:
            pFrame->res=F_CONST("unknown module");
            return false;
    }
    return res;
}

bool ComDispatch::dispatchCommonFrame(ComFrame * pFrame){
    return false;
}


bool ComDispatch::dispatchLedFrame(ComFrame * pFrame)
{
    LedCtrl * pLedCtrl;
    int res;
    switch(pFrame->index){
        case 0: pLedCtrl = pLedCtrl1; break;
        case 1: pLedCtrl = pLedCtrl2; break;
        default:
            pFrame->res = F_CONST("unknown index");
            return false;
    }
    if (pFrame->withPar == true){
        res = pLedCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pLedCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pLedCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchRgbLedFrame(ComFrame * pFrame)
{
    RgbLedCtrl * pRgbCtrl;
    switch(pFrame->index){
        case 0: pRgbCtrl = pRgbCtrl1; break;
        default:
            pFrame->res=F_CONST("unknown index");
            return false;
    }

    int res;
    if (pFrame->withPar == true){
        res = pRgbCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pRgbCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pRgbCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoStripeFrame(ComFrame * pFrame)
{
    NeoStripeCtrl * pStripeCtrl;
    switch(pFrame->index){
        case 0: pStripeCtrl = pNeoStripeCtrl1; break;
        case 1: pStripeCtrl = pNeoStripeCtrl2; break;
        default:
            pFrame->res=F_CONST("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pStripeCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pStripeCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pStripeCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame)
{
    NeoMatrixCtrl * pMatrixCtrl;
    switch(pFrame->index){
        case 0: pMatrixCtrl = pNeoMatrixCtrl1; break;
        case 1: pMatrixCtrl = pNeoMatrixCtrl2; break;
        default:
            pFrame->res=F_CONST("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pMatrixCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pMatrixCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pMatrixCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}
