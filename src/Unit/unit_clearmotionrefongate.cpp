//============================================================================
//  0x6F2855E0 - CUnit's slot-3 closure leaf.  `this` (ecx) is the unit,
//  one stack argument (a pointer whose own +0xC field is read - the
//  argument's wider layout is not established beyond that one field, so
//  it stays a raw offset rather than a named struct).  `retn 4`.
//
//  If the argument's +0xC pointer is set and equal to this unit's own
//  +0x2C4 motion reference (unit.h's m_pRef2C4, the same field
//  ClearMotionState releases), ask that object's own vtable slot 0x1B0
//  (no arguments past `this`); if that answers false, or it answers true
//  but SRefCountedAgent::QueryHitAllowed(candidate, 0, 4)
//  (Missile/missile_thunderbolt.h, 0x6F284EE0 - the same gate
//  CUnit::SubmitToPlayerTable's own sibling call uses, reinterpreting
//  `this` through the same layout) answers false, clear the motion state.
//============================================================================
#include "unit.h"
#include "missile_thunderbolt.h"

typedef int (__thiscall *Slot0x1B0Fn)(void* self);

void CUnit::ClearMotionRefOnHitGate(void* arg)
{
    void* candidate = *(void**)((char*)arg + 0xC);
    if (candidate && candidate == m_pRef2C4)
    {
        int result = ((Slot0x1B0Fn)(*(void***)candidate)[0x1B0 / 4])(candidate);
        if (!result ||
            !((SRefCountedAgent*)this)->QueryHitAllowed(candidate, 0, 4))
        {
            ClearMotionState();
        }
    }
}
