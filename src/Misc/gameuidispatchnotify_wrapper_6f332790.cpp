//============================================================================
//  CUnit batch-16 (slot-101/103 closure).
//
//  0x6F332790 - if the game UI exists, forward `this` into
//  SGameUIDispatchHost::NotifyDispatch3F4Slot110 (0x6F2F88B0,
//  gameuidispatchnotify_6f2f88b0.cpp) on it.
//============================================================================
#include "gameui.h"

struct SGameUIDispatchHost
{
    void NotifyDispatch3F4Slot110(void* arg);
};

void __fastcall ForwardToGameUIDispatch_6F332790(void* self)
{
    SGameUI* ui = (SGameUI*)GetGameUI(0, 0);
    if (ui != 0)
        ((SGameUIDispatchHost*)ui)->NotifyDispatch3F4Slot110(self);
}
