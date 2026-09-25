//============================================================================
//  0x6F2E5A70 - the out-of-line copy of unithandleref.h's resolve-and-notify
//  block, on CUnit's own +0x304 handle pair.  Reached by
//  CUnit::Deactivate (slot 13, unit_deactivate.cpp) with a real call while
//  the two pairs at +0x130/+0x13C get the same block inlined.
//
//  Own translation unit for exactly that reason, and because 0x6F2E5xxx is
//  a different module from Deactivate's own 0x6F282xxx.
//
//  The notify is a tail jump here (`jmp eax` off the resolved object's
//  vtable+0x5C) where the inlined copies leave it a call - the same
//  difference /O2 makes anywhere the notify is the last thing a function
//  does.
//============================================================================
#include "unit.h"
#include "unithandleref.h"

void CUnit::NotifyRef304Deactivated()
{
    ReleaseHandleRefNotify((SOptionalHandleRef*)&m_ref304);
}
