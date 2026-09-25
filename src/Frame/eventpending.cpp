//============================================================================
//  0x6F601D60 - CLayer::AddPendingState.
//
//  The same read-the-neighbour shape Misc/sweep_field_setters.cpp records for
//  0x6F626730 (`OrField1CCWithField1D0`): the new pending word is the old one
//  or'd with the mask *and* with the argument.  What is different here is the
//  tail: when the pending word actually gained bits, slot 61 is told which
//  ones, as a tail jump that rewrites this function's own argument slot.
//
//  The pair lives past CLayer's own 0xB4 bytes, in whichever derived
//  frame the 0x6F36xxxx panels build; the method is the base module's, so the
//  receiver is spelled through SFrameStateOwner rather than by inventing a
//  0x1D4-byte class for it.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CLayer::AddPendingState(int bits)
{
    SFrameStateOwner* state = (SFrameStateOwner*)this;

    int before = state->m_pendingState;
    int after = before | state->m_stateMask | bits;
    state->m_pendingState = after;

    int changed = after ^ before;
    if (changed)
        NotifyStateChanged(changed);
}
