//============================================================================
//  0x6F27A5A0 - CUnit::ClearMotionState.  Slot 13.  Was a naked thunk
//  (Misc/unreconstructed_thunks.cpp) until this session; unit.h's own note
//  on the declaration already described the shape correctly.
//
//  Release the +0x2C4 refcounted pointer (the manual
//  decrement-then-vtable-slot-0 expansion Misc/trivialaccessors_08.cpp's
//  RefHolder_* family already established, not a call out to
//  TRefCnt::Release() - the shipped stream calls slot 0 (`ReleaseSelf`),
//  not slot 1's `DeleteSelf`), zero the three motion floats at
//  +0x2B8/+0x2BC/+0x2C0 as one x87 store triple, then - if the object at
//  this unit's own +0x28 (SField28Owner, Misc/sweep_predicates.cpp) is
//  set - unbind its controller slot 3 with a null callback/context and a
//  zero blend (GameUI/controllerslotbind.cpp's own
//  BindControllerSlotDirect, one of its three byte-identical addresses).
//============================================================================
#include "unit.h"
#include "controllerslot.h"
#include "fvec3.h"

typedef void (__thiscall *RefDestroyFn)(void* self);

//  Misc/sweep_predicates.cpp (0x6F26B7F0).
struct SField28Owner { int __thiscall GetField28(); };

//  GameUI/controllerslotbind.cpp (0x6F4E86C0).
void __fastcall BindControllerSlotDirect(void* controller, int slot,
                                         void* callback, void* context,
                                         float blend);

void CUnit::ClearMotionState()
{
    void* ref = m_pRef2C4;
    if (ref != 0 && --*(int*)((char*)ref + 4) == 0)
        ((RefDestroyFn)(*(void***)ref)[0])(ref);
    m_pRef2C4 = 0;

    FVec3 zero = { 0.0f, 0.0f, 0.0f };
    *(FVec3*)&m_motion2B8 = zero;

    void* slotObj = (void*)((SField28Owner*)this)->GetField28();
    if (slotObj)
        BindControllerSlotDirect(slotObj, 3, 0, 0, 0.0f);
}
