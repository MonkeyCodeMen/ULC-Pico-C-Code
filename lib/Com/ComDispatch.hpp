#pragma once
#include "ComFrame.hpp"

class ComDispatch
{
public:
	ComDispatch();
	~ComDispatch() = default;

    bool dispatchFrame(ComFrame * pFrame);

private:
    bool dispatchCommonFrame(ComFrame * pFrame);
    bool dispatchLedFrame(ComFrame * pFrame);
    bool dispatchRgbLedFrame(ComFrame * pFrame);
    bool dispatchNeoStripeFrame(ComFrame * pFrame);
    bool dispatchNeoMatrixFrame(ComFrame * pFrame);

};
