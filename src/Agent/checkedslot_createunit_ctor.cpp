//============================================================================
//  0x6F095DD0 / 0x6F0C76E0 - SCheckedSlot_6F26C3E0's and
//  SCheckedSlot_6F2E59B0's constructors (checkedslot_createunit.h).
//
//  Own translation unit, separate from either type's own Assign: each
//  constructor issues a genuine `call sub_6F085E40`/`call sub_6F0C58F0`, so
//  the two must not be in a position to be inlined into each other.
//
//  The same __try/__finally every other constructor in this family has -
//  see checkedfogmodifierslot.cpp - for the same reason: Assign's virtual
//  GetAgileTypeId() call through `candidate` is the one thing here that can
//  raise, and the frame releases whatever half-assigned value is left if it
//  does.
//============================================================================
#include "checkedslot_createunit.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedSlot_6F26C3E0::SCheckedSlot_6F26C3E0(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

SCheckedSlot_6F2E59B0::SCheckedSlot_6F2E59B0(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
