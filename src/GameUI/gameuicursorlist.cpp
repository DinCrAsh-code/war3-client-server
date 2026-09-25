//============================================================================
//  0x6F38C690 - CWorldFrameWar3::GetTrackedWidgets.
//
//  Two instructions: the address of the member at +0x620.  A `lea` and not
//  a load, which is what taking the address of an *embedded* array gives and
//  what a pointer member would not.
//
//  Own translation unit: its one caller (0x6F381670) calls it for real.
//============================================================================
#include "gameui.h"

void* CWorldFrameWar3::GetTrackedWidgets()
{
    return &m_trackedWidgets;
}
