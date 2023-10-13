#pragma once
#include "ComFrame.hpp"

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
