//============================================================================
//  0x6F2B5CC0 - CItem::ArmDropTimer.
//
//  Called from CAbilityInventory::DetachAndNotify (abilityinventorydispatch.cpp)
//  once an item has been detached from a slot whose category record's two
//  field reads both came back non-zero and whose own +0x20 flag word has
//  both bit 9 (0x200) and bit 12 (0x1000) set: arm m_timer2 with the
//  category record's first field as the delay, event id 0xD01BE, and this
//  item as the timer's own subject.
//
//  Same shape as item_remove.cpp's own CItem::RemoveFromWorld arming
//  m_timer1 with event id 0xD01C1 - the identical thiscall member of the
//  timer sub-object (SAgentTimerArm::Arm, 0x6F4778F0), out of this call
//  tree and already redirected there.  Re-declared (not re-defined) here:
//  the struct/method declaration below has to be visible to call it, but
//  the naked body stays the one item_remove.cpp already defines, so the
//  two translation units share one symbol rather than link-conflicting
//  over two definitions of it.
//
//  The second stack argument is real (retn 8 cleans two pointers) but
//  unread inside this body - the shipped instruction stream loads only
//  `[esp+arg_0]` (the first-pushed-last, i.e. topmost, pointer) before the
//  add/call/retn; the caller's own second pointer is dead here regardless
//  of what it points at.
//============================================================================
#include "item.h"

//  0x6F4778F0 - declared, not defined, here; item_remove.cpp's own naked
//  body is the one definition this links against.
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

void CItem::ArmDropTimer(const CFloat* delay, const CFloat*)
{
    ((SAgentTimerArm*)&m_timer2)->Arm(delay, 0xD01BE, this, 0, 0);
}
