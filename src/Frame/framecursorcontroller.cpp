//============================================================================
//  0x6F606850 - CWorldFrameWar3::NotifyControllerChanged: swap the counted
//  reference at +0x140 for a new one.
//
//  The class and the name are the JASS_PauseGame slice's (GameUI/gameui.h),
//  which reached this address from the camera side and declared it there;
//  this file replaces the redirect that stood in for it.  Its +0x140 is
//  CFrame's own +0x140, which is what says a CWorldFrameWar3 is a CFrame.
//============================================================================
#include "gameui.h"
#include "refcnt.h"

void CWorldFrameWar3::NotifyControllerChanged(void* controller)
{
    //  m_140 is CFrame's own +0x140 - the field this file's header comment
    //  already called the controller slot.  Since CWorldFrameWar3 was
    //  re-based on CFrame (gameui.h) it carries CFrame's name and CFrame's
    //  `int` type, so the pointer traffic is spelled through a reference.
    void*& slot = (void*&)m_140;

    TRefCnt* previous = (TRefCnt*)slot;
    if (previous)
    {
        previous->Release();
        slot = 0;
    }

    TRefCnt* next = (TRefCnt*)controller;
    if (next)
        slot = next->AddRefOrNull();
}
