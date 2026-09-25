//============================================================================
//  0x6F602FE0 - CSimpleButton::SetEnabled.
//
//  Slot 28 is told either way; what the disabled path does first is drop
//  layer 4's priority back to zero (0x6F608C50, which tail-jumps into
//  MarkLayerDirty), and the two calls differ only in the first argument they
//  hand the slot.  Its own translation unit because the constructor calls
//  it.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleButton::SetEnabled(int enabled)
{
    if (enabled)
    {
        if (m_enabled == 0)
            NotifyEnabledChanged(1, 0);
        return;
    }

    if (m_enabled == 0)
        return;

    ResetLayerPriority(4);
    NotifyEnabledChanged(0, 0);
}
