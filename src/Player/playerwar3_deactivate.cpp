//============================================================================
//  0x6F415BB0 - CPlayerWar3::Deactivate, vtable slot 13 (+0x34).  Five
//  calls to the shared release helper (playerwar3_releasehelper.cpp) over
//  +0x27C, +0x280, +0x284, +0x290, +0x294 - the same five of the seven
//  "plain TRefCnt-shaped release" fields the not-yet-reconstructed
//  destructor also touches this same way (it additionally touches +0x288
//  and +0x28C, which this slot skips - see docs/targets/CPlayerWar3.md).
//  The last call is a tail `jmp`, which /O2 reproduces here the same way.
//============================================================================
#include "playerwar3.h"
#include "playerwar3_releasehelper.h"

void CPlayerWar3::Deactivate()
{
    CPlayerWar3_ReleaseAndClearSlot(&m_reserved27C);
    CPlayerWar3_ReleaseAndClearSlot(&m_reserved280);
    CPlayerWar3_ReleaseAndClearSlot(&m_reserved284);
    CPlayerWar3_ReleaseAndClearSlot(&m_reserved290);
    CPlayerWar3_ReleaseAndClearSlot(&m_reserved294);
}
