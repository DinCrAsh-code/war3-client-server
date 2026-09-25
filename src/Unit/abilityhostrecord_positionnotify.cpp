//============================================================================
//  0x6F2DB8C0 / 0x6F2DB850 / 0x6F2E3B10 - the "tell `unit` about this
//  record's world position" chain: NotifyPositionIfVisible guards
//  ResolveAndNotifyPosition, which resolves `unit`'s own path position and
//  hands it to the out-of-scope 0x6F2DAA40 before forwarding into
//  NotifyPositionEvent (all three declared on SAbilityHostRecord,
//  abilityhostrecord.h).
//============================================================================
#include "abilityhostrecord.h"
#include "unit.h"
#include "game.h"
#include "missile_thunderbolt.h"
#include "widget.h"

//----------------------------------------------------------------------------
//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (Agent/handlereref_resolve.cpp); re-declared here the way
//  every other caller in this codebase does, so the mangled name binds to
//  that one real symbol.
//----------------------------------------------------------------------------
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  CUnit's own event-forwarding face - the target/receiver role
//  NotifyPositionEvent calls `unit` through, the same locally-redeclared
//  struct abilityhostrecord_notifyobservers.cpp and
//  unit_abilityhostnotify.cpp both already use for 0x6F2AB3E0.
struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);
};

//----------------------------------------------------------------------------
//  0x6F2DAA40 - out of scope: its own `__except_handler4`-shaped SEH frame
//  (docs/msvc-vc8-idioms.md) makes it a destructible-local case this
//  toolchain cannot reproduce, the same reason 0x6F2DBB90 itself stays a
//  redirect.  `this` is the SAbilityHostRecord, `retn 0Ch` (three stack
//  arguments): the unit's own +0x30 rawcode, a pointer to the freshly
//  copied CWorldVec3, and a pointer four bytes into that same copy - both
//  addresses the caller's own stack frame, modelled as raw `void*` since
//  what 0x6F2DAA40 itself does with them is not this call tree's to know.
//----------------------------------------------------------------------------
struct SUnknownFactory_6F2DAA40
{
    void* Create(unsigned int rawcode, const void* fullVec, const void* tailVec);
};
__declspec(naked) void* SUnknownFactory_6F2DAA40::Create(unsigned int, const void*, const void*)
{
    __asm { mov eax, 06F2DAA40h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F2EC850 - out of scope, not otherwise identified.  `this` is the
//  object NotifyPositionEvent's own +0x38-chained owner resolves through
//  its +0x30 pair; `retn 4`, one stack argument (`unit`).
//----------------------------------------------------------------------------
struct SUnknownNotify_6F2EC850
{
    void Notify(void* unit);
};
__declspec(naked) void SUnknownNotify_6F2EC850::Notify(void*)
{
    __asm { mov eax, 06F2EC850h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F2E61B0 - out of scope, not otherwise identified.  `retn 4`, one
//  stack argument (always 0 at this call site).
//----------------------------------------------------------------------------
struct SUnknownNotify_6F2E61B0
{
    void Notify(int flag);
};
__declspec(naked) void SUnknownNotify_6F2E61B0::Notify(int)
{
    __asm { mov eax, 06F2E61B0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  Whatever CUnit::ResolveRef304() resolves to for NotifyPositionEvent -
//  not SAbilityHostRecord itself (its own known offsets don't line up with
//  +0x30/+0x34/+0x3C), and not otherwise identified.  Modelled with raw
//  offsets, the same treatment abilityhostrecord_handlerefs.cpp gives its
//  own not-otherwise-identified fields.
//----------------------------------------------------------------------------
struct SUnknownRef304Owner
{
    char               m_reserved00[0x30];
    SOptionalHandleRef m_ref30;                // +0x30/+0x34
    char               m_reserved38[0x3C - 0x38];
    SOptionalHandleRef m_ref3C;                // +0x3C/+0x40
};

//----------------------------------------------------------------------------
//  0x6F2DB850
//----------------------------------------------------------------------------
void* SAbilityHostRecord::ResolveAndNotifyPosition(CUnit* unit)
{
    CWorldVec3 position = ((CPathRef*)unit->GetHandleObject())->SmartPtrToWorldVec3();
    CWorldVec3 copy = position;

    unsigned int rawcode = *(unsigned int*)((char*)unit + 0x30);
    void* made = ((SUnknownFactory_6F2DAA40*)this)->Create(
        rawcode, &copy, (const char*)&copy + 4);

    ((SAbilityHostRecord*)made)->NotifyPositionEvent(unit);
    return made;
}

//----------------------------------------------------------------------------
//  0x6F2DB8C0
//----------------------------------------------------------------------------
void SAbilityHostRecord::NotifyPositionIfVisible(CUnit* unit)
{
    if (*(unsigned int*)((char*)unit + 0x20) & 0xD8000)
        return;
    if (unit->QueryVisibleImpl(1))
        return;
    if (unit->TestFootprintTypeRangeAndFlagBit30Clear())
        return;
    if (unit->m_int248 & 0x10)
        return;

    ResolveAndNotifyPosition(unit);
}

//----------------------------------------------------------------------------
//  0x6F2E3B10
//----------------------------------------------------------------------------
void SAbilityHostRecord::NotifyPositionEvent(CUnit* unit)
{
    ((SEventNotifyOrForwardHost*)unit)->NotifyOrForwardEvent(this, 1);
    ((CWidget*)unit)->PostAgentEvent2(this, 1);
    ((SOptionalHandleRefResolver*)&m_ref38)->ResolveChained(unit);

    m_counter44 += 1;

    SUnknownRef304Owner* owner = (SUnknownRef304Owner*)unit->ResolveRef304();
    if ((owner->m_ref30.m_typeTag & (int)owner->m_ref30.m_handle) != -1)
    {
        void* resolved = QueryHandleField0x54(&owner->m_ref30);
        if (resolved != 0)
            ((SUnknownNotify_6F2EC850*)resolved)->Notify(unit);
    }

    ((SUnknownNotify_6F2E61B0*)owner)->Notify(0);
    ((SOptionalHandleRefResolver*)&owner->m_ref3C)->ResolveChained(this);
}
