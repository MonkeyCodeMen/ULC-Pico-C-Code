#pragma once
#include "ComFrame.hpp"

#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>
#include <NeoStripeCtrl.hpp>

extern LedCtrl    * pLedCtrl1;
extern LedCtrl    * pLedCtrl2;
extern RgbLedCtrl * pRgbCtrl1;
extern NeoStripeCtrl  * pNeoStripeCtrl1;
extern NeoStripeCtrl  * pNeoStripeCtrl2;



class ComDispatch
{
public:
	ComDispatch();
	~ComDispatch() = default;

    void dispatchFrame(ComFrame * pFrame);

private:
    void dispatchCommonFrame(ComFrame * pFrame);
    void dispatchLedFrame(ComFrame * pFrame);
    void dispatchRgbLedFrame(ComFrame * pFrame);
    void dispatchNeoStripeFrame(ComFrame * pFrame);
    void dispatchNeoMatrixFrame(ComFrame * pFrame);

};
