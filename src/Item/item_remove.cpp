//============================================================================
//  0x6F2B8270 - CItem::RemoveFromWorld.
//
//  The removal path CItem::OnValueFloorReached (item_valuefloor.cpp) tail-
//  jumps into once an item's tracked value has hit its floor: drop the
//  shadow sprite, run slot 102's own selection refresh, file the two
//  rendering-side records, hand the item to whatever owns its placement ref
//  (falling back to a local detach when nothing does), and then either
//  bounce straight out through slot 23 if the item is already pending, or
//  release the footprint and arm the death-effect timer.
//
//  `.\\CItem.cpp` and line 0x20C (524) are the shipped source's own file and
//  line, passed to the rendering-side recorder - the only place in this
//  family that names the original translation unit outright.
//
//  Own translation unit: seven real calls, two of them virtual, and its one
//  caller reaches it with a tail jump.
//============================================================================
#include "item.h"
#include "gamecontext.h"        // GetConfigFloat
#include "itemhandleresolve.h"  // QueryHandleField0x54
#include "itemhandledispatch.h" // SItemHandleFieldObject::DispatchUnknown

//  0x6F2B71B0 - build or drop the item's shadow sprite (a `Misc`/
//  `ItemShadowFile` lookup, the sprite's world transform off slot 46 and
//  two more config floats, all behind a /GS frame cookie).  A thiscall
//  member with one argument, always 0 here - which is the branch that only
//  destroys whatever shadow already exists.  Not reconstructed; redirected.
//
//  0x6F333330 - a second rendering-side gate, thiscall with no arguments,
//  sibling of the 0x6F333290 one CItem::SetPending uses (item_pending.cpp).
//
//  0x6F2B7E60 - the local fallback detach, thiscall with no arguments.
struct SItemRemoveHelpers
{
    void SetShadowEnabled(int enabled);
    void FileRenderRecord();
    void DetachLocally();
};

__declspec(naked) void SItemRemoveHelpers::SetShadowEnabled(int)
{
    __asm
    {
        mov     eax, 06F2B71B0h
        jmp     eax
    }
}

__declspec(naked) void SItemRemoveHelpers::FileRenderRecord()
{
    __asm
    {
        mov     eax, 06F333330h
        jmp     eax
    }
}

__declspec(naked) void SItemRemoveHelpers::DetachLocally()
{
    __asm
    {
        mov     eax, 06F2B7E60h
        jmp     eax
    }
}

//  0x6F333260 - the rendering-side "this item is going away, recorded from
//  <file>:<line>" note.  __fastcall (`this` in ecx, the file string in edx)
//  with the line number as its one stack argument - not a thiscall member,
//  which would have put the string on the stack too and cleaned eight
//  bytes instead of four.
void __fastcall RecordItemRemoval(void* item, const char* file, int line);

__declspec(naked) void __fastcall RecordItemRemoval(void*, const char*, int)
{
    __asm
    {
        mov     eax, 06F333260h
        jmp     eax
    }
}

//  0x6F4778F0 - arm a CAgentTimer: (delay, event id, subject, 0, 0).  A
//  thiscall member of the timer (`this` = &m_timer1, `retn 14h`).  Out of
//  this call tree; redirected.
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

__declspec(naked) void SAgentTimerArm::Arm(const CFloat*, int, void*, int, int)
{
    __asm
    {
        mov     eax, 06F4778F0h
        jmp     eax
    }
}

//  slot 102 (+0x198) - CItem::RefreshSelectionState (item_selectionrefresh.cpp),
//  reached through the vtable because the shipped code dispatches it.
typedef void (__thiscall *Slot0x198Fn)(void* self);
//  slot 83 (+0x14C) - CWidget::RemoveFootprint.
typedef void (__thiscall *Slot0x14CFn)(void* self, int blocking, int useAlternate);
//  slot 23 (+0x5C) - CItem::Method_0x5C, reached as a tail jump.
typedef void (__thiscall *Slot0x5CFn)(void* self);

void CItem::RemoveFromWorld()
{
    ((SItemRemoveHelpers*)this)->SetShadowEnabled(0);
    ((Slot0x198Fn)(*(void***)this)[0x198 / 4])(this);
    RecordItemRemoval(this, ".\\CItem.cpp", 0x20C);
    ((SItemRemoveHelpers*)this)->FileRenderRecord();

    //  Read *before* the placement-ref work, and kept in a callee-saved
    //  register across all of it: the shipped code loads m_flags and masks
    //  bit 0 up at the top (`mov edi,[esi+20h]` / `and edi,1`) and only
    //  tests it at the bottom.  Written down there instead, MSVC reloads
    //  m_flags after the calls, because none of them can be proven to
    //  leave it alone.
    int pending = m_flags & 1;

    //  The same `(handle & typeTag) != -1` sentinel test every other
    //  reader of a {handle, typeTag} pair in this codebase uses
    //  (itemhandleresolve.h).
    SOptionalHandleRef* ref = &m_position.m_placementRef;
    int claimed = 0;
    if ((int)(ref->m_handle & ref->m_typeTag) != -1)
    {
        void* owner = QueryHandleField0x54(ref);
        if (owner != 0)
            claimed = ((SItemHandleFieldObject*)owner)->DispatchUnknown(this);
    }
    if (!claimed)
        ((SItemRemoveHelpers*)this)->DetachLocally();

    //  Written as "not pending does the work, pending falls out of the
    //  bottom" and not the other way round: the shipped code keeps the
    //  long path as the fall-through and leaves the slot-23 call in tail
    //  position, where MSVC turns it into a `jmp`.  Spelled as
    //  `if (pending) { slot23(); return; }` instead, the same two blocks
    //  come out in the opposite order and the tail jump becomes a call.
    if (!pending)
    {
        ((Slot0x14CFn)(*(void***)this)[0x14C / 4])(this, 1, 1);

        CFloat delay = GetConfigFloat("Misc", "EffectDeathTime", 0);
        ((SAgentTimerArm*)&m_timer1)->Arm(&delay, 0xD01C1, this, 0, 0);
        return;
    }

    ((Slot0x5CFn)(*(void***)this)[0x5C / 4])(this);
}
