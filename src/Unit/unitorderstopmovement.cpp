//============================================================================
//  0x6F2A49C0 - CUnit::StopMovement: called on the way out of an order (both
//  StartOrderNow and AppendOrder reach it through CancelCurrentOrder /
//  FinishCurrentOrder's own callees) to re-publish the movement sub-object's
//  classification and drop the two flag bits that say a ramp/tracking
//  timer is still owed.
//
//  Body, in order:
//    * ApplyMovementClassificationParams() - re-publish +0x164's own
//      classification (unit_applymovementclassificationparams.cpp);
//    * if +0x54 (the pending-ramp reference count) is already zero,
//      FlushPendingRamp() right away rather than waiting for it to reach
//      zero on its own;
//    * if +0x5C bit 21 (0x200000) is set, RestartPropertyRamp(), then
//      decrement +0x54 and clear that same bit;
//    * unconditionally clear +0x5C bit 20 (0x100000) - the bit
//      PrepareForNewOrder tests before calling this;
//    * CUnit_DecrementField198AndTail(this) - the order-queue-count
//      decrement (+0x198) already reconstructed as a naked leaf
//      (Misc/cunit_agent6_naked.cpp), which itself tails into
//      ApplyMovementClassificationParams again;
//    * PopPendingTask(), then tail-call
//      MsgHandler_6F29ED60(this) - unit_dispatch.cpp's own redirect.
//
//  Own translation unit: the two calls that are real reconstructions here
//  (ApplyMovementClassificationParams, RestartPropertyRamp) are declared
//  through unit.h; CUnit_DecrementField198AndTail is the naked leaf from
//  Misc/cunit_agent6_naked.cpp, and PopPendingTask/MsgHandler_6F29ED60 are
//  its own tail chain, unchanged.
//============================================================================
#include "unit.h"

//  0x6F2A3CD0 - Misc/cunit_agent6_naked.cpp.  `this` in ecx, no stack args.
//  Redeclared here rather than through a header, matching the same
//  redeclaration unitrampflush.cpp's own callees use - the mangled name is
//  the same symbol as the real definition (plain __fastcall(void*), no
//  `extern "C"`), not a second one.
void __fastcall CUnit_DecrementField198AndTail(void*);

void CUnit::StopMovement()
{
    ApplyMovementClassificationParams();

    if (m_int54 == 0)
        FlushPendingRamp();

    if ((m_flags5C & 0x200000) != 0)
    {
        RestartPropertyRamp();
        m_int54 -= 1;
        m_flags5C &= 0xFFDFFFFF;
    }

    m_flags5C &= 0xFFEFFFFF;

    CUnit_DecrementField198AndTail(this);
    PopPendingTask();
    MsgHandler_6F29ED60();
}
