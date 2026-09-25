//============================================================================
//  0x6F2B8330 - CItem::OnValueFloorReached.
//
//  Not a vtable slot: the shared tail both of CItem's value setters
//  (item_valueset.cpp, slots 73 and 74) run once the tracked value has just
//  crossed down onto its floor.  It pins the value at exactly zero, drops
//  the item's pathing shadow flag, tells every target that was following it
//  that it is gone, fires the death event, plays animation 1, releases the
//  footprint and tail-jumps into the removal path.
//
//  Own translation unit: six real calls out of it, two of them virtual, and
//  its own two callers reach it with a `call` rather than inlined.
//============================================================================
#include "item.h"

//  0x6F2AD4D0 - build a CEvent record on the stack (vftable
//  ??_7CEvent@@6B@, id 0xD01A0, subject `this`) after poking the local
//  player's world-object slot, and dispatch it through vtable slot 4.  A
//  thiscall member with no arguments; not reconstructed - it carries the
//  same __except_handler4-shaped SEH frame this toolchain cannot reproduce
//  that CWidget::NotifyTargetLost (widget_targetlost.cpp) is redirected
//  for, so it is redirected the same way.
struct SItemDeathEvent
{
    void Fire();
};

__declspec(naked) void SItemDeathEvent::Fire()
{
    __asm
    {
        mov     eax, 06F2AD4D0h
        jmp     eax
    }
}

//  slot 35 (+0x8C) - CWar3Image::PlayAnimation, reached through the vtable
//  rather than by name because CItem does not override it and the shipped
//  code dispatches it.
typedef float (__thiscall *Slot0x8CFn)(void* self, unsigned int name, int flags, int notify);
//  slot 83 (+0x14C) - CWidget::RemoveFootprint.
typedef void (__thiscall *Slot0x14CFn)(void* self, int blocking, int useAlternate);

void CItem::OnValueFloorReached()
{
    //  The notify goes through FloatMini's own vtable slot 0 with the
    //  shared encoded zero, not through a plain store: the value has
    //  watchers, and pinning it is what tells them.
    ((FloatMiniB::NotifyFn)(*(void***)&m_floatB)[0])(
        &m_floatB, (CFloat*)&g_CFloatZero, 1);

    m_position.SetTopByteFlag80(1);
    NotifyTargetLost(-1, -1);
    ((SItemDeathEvent*)this)->Fire();

    //  The animation's own length comes back in st0 and is thrown away -
    //  the shipped `fstp st` is what MSVC emits for an unused float return,
    //  not a source-level discard of any kind.
    ((Slot0x8CFn)(*(void***)this)[0x8C / 4])(this, 1, 0, 1);

    ((Slot0x14CFn)(*(void***)this)[0x14C / 4])(this, 1, 1);

    RemoveFromWorld();
}
