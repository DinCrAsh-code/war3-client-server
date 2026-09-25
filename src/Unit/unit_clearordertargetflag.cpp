//============================================================================
//  0x6F277D20 - CUnit::ClearOrderTargetExecutingFlag.
//
//  Slot-3 (Method_0x0C) closure: reached from CUnit::PopPendingTask
//  (unit_taskpop.cpp) for the handful of order ids that need the target the
//  order was aimed at told "I'm not being ordered against any more" before
//  the task node itself is released. `this`, no stack arguments, `retn 0`.
//
//  The target is the same {handle, typeTag} pair unitorder.h's own
//  SUnitOrderState.m_orderTargetRef names (+0x19C/+0x1A0) - reused here as
//  unit.h's own m_ref19C rather than duplicating the field. Whatever it
//  resolves to is not otherwise identified in this tree (no dump reaches
//  its own class), so it is read through a raw offset the same way
//  Agent/handlereref_resolve.cpp treats its own not-yet-typed object.
//============================================================================
#include "unit.h"
#include "itemhandleresolve.h"

void CUnit::ClearOrderTargetExecutingFlag()
{
    if ((m_ref19C.m_b & (int)m_ref19C.m_t) == -1)
        return;

    void* target = QueryHandleField0x54((SOptionalHandleRef*)&m_ref19C);
    if (!target)
        return;

    //  Bit 2 of the resolved object's own +0x20 flags word.
    *(unsigned int*)((char*)target + 0x20) &= 0xFFFFFFFBu;
}
