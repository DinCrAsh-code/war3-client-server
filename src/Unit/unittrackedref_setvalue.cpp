//============================================================================
//  0x6F477350 - SUnitTrackedRef::SetValue: publish a new value on the
//  resolved range object.
//
//  Was a thunk (unreconstructed_thunks.cpp).  This is the write half of
//  the pair whose read half is SUnitTrackedRef::GetValue (0x6F4773A0,
//  unittrackedref.cpp), and it is what JASS's SetUnitState(UNIT_STATE_MANA)
//  and CUnit::SetLife both end in.
//
//  **A set spelled as an add**, and that is the whole point of the body:
//  the object underneath is a *ramp*, a value that moves on its own with
//  game time, so "set it to v" has no meaning until you say when.  The
//  shipped code answers that by evaluating the ramp at *now* and then
//  applying the difference:
//
//      now      = GetTravelDistance()          // how far time has moved
//      current  = RampValueAt(now)             // where the value is now
//      AddToValue(v - current)                 // ... so it lands on v
//
//  which is arithmetically `= v` and is not written that way anywhere,
//  because AddToValue is also what carries the old and new value into
//  NotifyRangeChanged.  Going straight at m_value would set it and tell
//  nobody.
//
//  Two orderings are load-bearing:
//
//   * `current` is bound as a *reference* to what RampValueAt filled, not
//     copied into a named CFloat.  The shipped code pushes that buffer's
//     address straight into operator-'s argument list - the same idiom
//     pathtraceramp.cpp and unittrackedref.cpp's GetValue already record
//     for this pair.
//
//   * the subtraction is `*value - current`, in that order.  operator-
//     negates its *second* operand (`xor esi, 80000000h`) and the shipped
//     body negates the ramp reading, not the caller's.  Reversing it would
//     compile and would set the value to `current - v`.
//
//  Own translation unit: it calls GetValue's two helpers and AddToValue for
//  real, so it may not be folded in with either.
//============================================================================
#include "unittrackedref.h"
#include "handleobjectrange.h"
#include "game.h"
#include "unit.h"

void SUnitTrackedRef::SetValue(const CFloat* value)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    const CFloat& travelled = trace->GetTravelDistance();

    CFloat current(kCFloatNoInit);
    trace->RampValueAt(&current, &travelled);

    //  A third named binding, not `AddToValue(&(*value - current))`.  The
    //  shipped frame is `sub esp, 12` - three CFloat slots, one each for
    //  the travelled distance, the ramp reading and the difference - and
    //  folding the last one into the call argument makes MSVC reuse a slot
    //  and carve only eight.
    const CFloat& delta = *value - current;

    ((SHandleObjectRangeImpl*)trace)->AddToValue(&delta);
}
