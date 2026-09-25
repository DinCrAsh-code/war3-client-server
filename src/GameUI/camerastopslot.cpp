//============================================================================
//  0x6F304190 - CCameraWar3::StopSlot: stop whichever camera property a
//  controller slot index names, releasing its modifier(s) if a live handle
//  still backs them.  `retn 4`.
//
//  A ten-case switch (0..9), each case releasing the named property's own
//  m_pModifier and m_pModifier2 pair with the same six instructions apiece -
//  test the handle still resolves, run the modifier's own slot 2 ("stop
//  driving whatever you are driving", the same dispatch ReleasePanModifiers
//  already uses), then drop the reference and null the field - except slot 6
//  (m_targetZ), which only ever populates m_pModifier and so only tears that
//  one down.  Reproduced as one `__forceinline` helper rather than ten
//  hand-copies: at this build's /O2 an inlined call collapses back down to
//  exactly the repeated straight-line shape the dump has.
//
//  The slot -> property mapping came from the dump's own field offsets
//  (case body writes [this+0xDC]/[this+0xE0] for slot 8, which is
//  m_listenerDistance's own +0x10/+0x14 modifier pair, and so on for every
//  other case) rather than from any naming already in this tree:
//    0 targetDistance, 1 farZ, 2 angleOfAttack, 3 fieldOfView, 4 roll,
//    5 rotation, 6 targetZ (single), 7 nearZ, 8 listenerDistance,
//    9 listenerAngle.
//
//  Was a naked `mov eax,<addr>` / `jmp eax` thunk in gameui_thunks.cpp
//  (declared `retn 4`, which this body's own stack argument now proves
//  right); replaced now that the real body is reconstructed.
//
//  Own translation unit: CCameraWar3::StopAll (camerastopall.cpp) reaches
//  it with ten real calls.
//============================================================================
#include "gameui.h"

//  0x6F03FA30 - LookupHandle (Agent/handle.cpp): resolve a handle only if
//  the object it names still carries the given type tag.  The modifier
//  object's own handle/typeTag pair sit at +0x08/+0x0C - not the same
//  offsets SModifierSubject documents (positionmodifier.h, +0x0C/+0x10),
//  so this is a different, undeclared record and read through raw offsets
//  rather than through a named type this pass has no other evidence for.
class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

typedef void (__thiscall *ModifierStopSlotFn)(void*);

//  Release one modifier reference: run its own "stop" dispatch (vtable slot
//  2) if a live handle still backs it, then drop the reference and clear
//  the field either way.
static __forceinline void ReleaseModifierSlot(TRefCnt*& slot)
{
    TRefCnt* mod = slot;
    if (mod != 0)
    {
        unsigned int handle = *(unsigned int*)((char*)mod + 0x08);
        int typeTag = *(int*)((char*)mod + 0x0C);
        if (LookupHandle(handle, typeTag) != 0)
        {
            void** vtable = *(void***)mod;
            ((ModifierStopSlotFn)vtable[2])(mod);
        }
    }

    if (mod != 0)
    {
        if (--mod->m_refcount == 0)
            mod->ReleaseSelf();
    }

    slot = 0;
}

//  Case bodies are written in the shipped jump table's own memory order
//  (8, 9, 0, 1, 6, 2, 3, 4, 5, 7 - read straight off each case block's own
//  address in the dump), not in ascending slot order: this compiler lays a
//  dense switch's case bodies out in source order behind the jump table,
//  so matching the body order is what makes the jump table itself line up
//  with the shipped one instead of just being permuted underneath it.
void CCameraWar3::StopSlot(int slot)
{
    switch (slot)
    {
    case 8:
        ReleaseModifierSlot(m_listenerDistance.m_pModifier);
        ReleaseModifierSlot(m_listenerDistance.m_pModifier2);
        break;
    case 9:
        ReleaseModifierSlot(m_listenerAngle.m_pModifier);
        ReleaseModifierSlot(m_listenerAngle.m_pModifier2);
        break;
    case 0:
        ReleaseModifierSlot(m_targetDistance.m_pModifier);
        ReleaseModifierSlot(m_targetDistance.m_pModifier2);
        break;
    case 1:
        ReleaseModifierSlot(m_farZ.m_pModifier);
        ReleaseModifierSlot(m_farZ.m_pModifier2);
        break;
    case 6:
        ReleaseModifierSlot(m_targetZ.m_pModifier);
        break;
    case 2:
        ReleaseModifierSlot(m_angleOfAttack.m_pModifier);
        ReleaseModifierSlot(m_angleOfAttack.m_pModifier2);
        break;
    case 3:
        ReleaseModifierSlot(m_fieldOfView.m_pModifier);
        ReleaseModifierSlot(m_fieldOfView.m_pModifier2);
        break;
    case 4:
        ReleaseModifierSlot(m_roll.m_pModifier);
        ReleaseModifierSlot(m_roll.m_pModifier2);
        break;
    case 5:
        ReleaseModifierSlot(m_rotation.m_pModifier);
        ReleaseModifierSlot(m_rotation.m_pModifier2);
        break;
    case 7:
        ReleaseModifierSlot(m_nearZ.m_pModifier);
        ReleaseModifierSlot(m_nearZ.m_pModifier2);
        break;
    }
}
