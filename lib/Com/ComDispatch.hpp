#pragma once
#include "ComFrame.hpp"
#include <Ctrl.hpp>

class ComDispatch
{
public:
	ComDispatch();
	~ComDispatch() = default;

    bool dispatchFrame(ComFrame * pFrame);

private:
    bool _dump(ComFrame * pFrame);
    bool _dispatchLedFrame(ComFrame * pFrame);
    bool _dispatchCommonFrame(ComFrame * pFrame);

    Ctrl * _refToObj(char module,uint8_t index);

};
