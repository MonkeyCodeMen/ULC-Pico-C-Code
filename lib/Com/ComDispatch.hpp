#pragma once
#include "ComFrame.hpp"

#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>

extern LedCtrl    * pLedCtrl1;
extern LedCtrl    * pLedCtrl2;
extern RgbLedCtrl * pRgbCtrl1;

class ComDispatch
{
public:
	ComDispatch();
	~ComDispatch() = default;

    void dispatchFrame(ComFrame * pFrame);

private:
    void dispatchLedFrame(ComFrame * pFrame);
    void dispatchRgbLedFrame(ComFrame * pFrame);
    void dispatchNeoStripeFrame(ComFrame * pFrame);
    void dispatchNeoMatrixFrame(ComFrame * pFrame);

};
