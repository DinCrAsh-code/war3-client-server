//============================================================================
//  0x6F40FE30 / 0x6F40FD30 / 0x6F40FDB0 - three near-identical dispatchers
//  the unit factory (0x6F29F990, jasscreateunit.h) calls as it finishes a
//  newly created unit, one of the three picked by which classification bit
//  the unit's own type carries:
//
//      6F29FC64    test    bl, 1
//      6F29FC6E    call    sub_6F40FE30
//      6F29FC7D    test    bl, 2
//      6F29FC87    call    sub_6F40FD30
//      6F29FC8E    test    bl, 4
//      6F29FC98    call    sub_6F40FDB0
//
//  `this` for all three is the same player record `sub_6F3A1650`
//  (`IndexedArrayHolder::At`) hands back - the same object
//  Game/playerunitcounters.cpp already views as `SPlayerUnitCounters` and
//  this header views as `SPlayerScore`.  Two arguments, `retn 8`: the unit
//  type id (a four-character code, from the hash the tally-list lookup
//  underneath runs over it) and a flag, forwarded to the tally bump
//  untouched.
//
//  The three differ only in which `CPlayerTallyList::TallyCounterAdjustX`
//  they call - see playerscore.h's own note on those for why that one call
//  is a redirect rather than a reconstruction.  Everything else - the
//  event fire, the GetGameUI dirty-bit poke - is byte-identical across all
//  three, which is exactly what "near-identical siblings" means here: not
//  three copies of one function, but one three-instruction-different body
//  the shipped compiler happened to lay out at three separate addresses
//  rather than folding (no `/OPT:ICF` in this build - the same reason
//  Math/cfloatnearlyequal.cpp needs its own file for a second copy of
//  `CFloatNearlyEqual` with a different constant).
//
//  One translation unit for the three: consecutive addresses (0x100 bytes
//  apart), same receiver, same shape, and none calls another.
//============================================================================
#include "playerscore.h"
#include "unitorder.h"        // g_vftCEvent

//  0x6F300710 - the lazily-constructed CGameUI singleton.  Behind an
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce, so
//  it stays a redirect (Widget/selectable_attachcreate.cpp); declared here,
//  not re-thunked - a second naked body at the same address is the
//  two-symbols-for-one-function defect docs/notes/verifier-gate-link-
//  failures.md warns about.
struct SGameUI
{
    char          m_reserved00[0x2A0];
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F2F3220 - already reconstructed as
//  SField284Owner::OrField284SetField26C (Misc/misc_trivial_getters.cpp).
struct SField284Owner
{
    void __thiscall OrField284SetField26C(unsigned int mask);
};

void SPlayerScore::TallyOnUnitCreated1(unsigned int typeId, int flag)
{
    m_pTallies->TallyCounterAdjustA(typeId, flag);
    FireTallyChangedEvent();

    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SField284Owner*)ui)->OrField284SetField26C(0);
}

void SPlayerScore::TallyOnUnitCreated2(unsigned int typeId, int flag)
{
    m_pTallies->TallyCounterAdjustB(typeId, flag);
    FireTallyChangedEvent();

    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SField284Owner*)ui)->OrField284SetField26C(0);
}

void SPlayerScore::TallyOnUnitCreated3(unsigned int typeId, int flag)
{
    m_pTallies->TallyCounterAdjustC(typeId, flag);
    FireTallyChangedEvent();

    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SField284Owner*)ui)->OrField284SetField26C(0);
}

//----------------------------------------------------------------------------
//  0x6F40B540 - build a CEvent record on the stack (vftable ??_7CEvent@@6B@,
//  id 0x80261, no argument) and dispatch it through vtable slot 4 - the
//  same shape SGenericWorldEvent::Fire (0x6F40B5A0, Item/itemgenericevent
//  .cpp) and SItemDeathEvent::Fire (0x6F2AD4D0, Item/item_valuefloor.cpp)
//  carry, each with its own id and argument count, and the same shape
//  CAgentRegistrySingleton::RebuildDayNightListeners (Agent/agentdaynight
//  .cpp) already reconstructs for real with an identical
//  `((AnnounceEventFn)(*(void***)this)[0x10/4])(this, &event)` dispatch -
//  that is the pattern followed here instead of leaving this address a
//  third bare redirect.  `this` (SPlayerScore) is a view onto a real
//  polymorphic object, which is what makes `*(void***)this` a genuine
//  vtable read rather than reading SPlayerScore's own first data member.
//
//  Field write order is the shipped store order, not struct declaration
//  order (docs/msvc-vc8-idioms.md, "Statements are not reordered"):
//  +0x04 first, then the vtable word, then the message id, then +0x0C -
//  four separate statements in that literal sequence.
//----------------------------------------------------------------------------
struct STallyChangedEvent
{
    const void*  m_vtable;      // +0x00
    int          m_reserved04;  // +0x04
    unsigned int m_msgId;       // +0x08
    int          m_reserved0C;  // +0x0C
};

typedef void (__thiscall *AnnounceEventFn)(void*, STallyChangedEvent*);

void SPlayerScore::FireTallyChangedEvent()
{
    STallyChangedEvent event;
    event.m_reserved04 = 0;
    event.m_vtable = &g_vftCEvent;
    event.m_msgId = 0x80261;
    event.m_reserved0C = 0;

    ((AnnounceEventFn)(*(void***)this)[0x10 / 4])(this, &event);
}

//----------------------------------------------------------------------------
//  CPlayerTallyList::TallyCounterAdjustA/B/C (0x6F403890/0x6F403770/
//  0x6F403810) are all defined in Player/playerscore_tallycounteradjust.cpp,
//  their own module's TU (address-adjacent to each other, not to this
//  file's own 0x6F40Fxxx/0x6F40B5xx addresses) - declared through
//  playerscore.h as usual, not re-thunked here.
//----------------------------------------------------------------------------
