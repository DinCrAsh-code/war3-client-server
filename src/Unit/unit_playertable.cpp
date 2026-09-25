//============================================================================
//  0x6F285080 / 0x6F285630 / 0x6F285A40 - CUnit's vtable slots 63 (+0xFC),
//  98 (+0x188) and 97 (+0x184), the 0x6F285xxx module.  See unit.h.
//
//  Slot 63 first.
//
//  CWidget's own body at this slot (0x6F2AD680, widgetregisterquery.cpp)
//  submits the widget's *position*; CUnit's submits the unit itself, and
//  answers whether anything took it.
//
//  Two early outs before any of that: no player-slot table at all, or the
//  table's own +0x3E0 not set, and the answer is no.  Then the same
//  registry-entry probe CWidget's body does - entry 13, one indirection
//  through its +0x10 and one more through that, bit 0x200 of the result's
//  +0x04 - folded into the caller's flag word as bit 0.
//
//  This slot is `int` in the base as well.  CWidget's body leaves whatever
//  SPlayerTable::SubmitWidget returned in eax and never touches it after,
//  which is a `return` of a call, not a void function - and CUnit's makes
//  that unambiguous by computing 0 or 1 for itself.
//============================================================================
#include "unit.h"
#include "widgetregistersubmit.h"
#include "gamecontext.h"

//  Also reached by widgetregisterquery.cpp, which does the same probe: the
//  player-slot table global and the thread-local game-data block.
//  GetThreadLocalSlot is gamecontext.h's, under its real name - a local
//  re-declaration under a different one compiles and links fine and
//  silently costs the call site its match, because the canonicaliser
//  compares symbol names.
extern void* g_unk6FAB65F4;

//  0x6F26F9E0 - misc_trivial_getters.cpp.  Its `this` here is the CUnit
//  itself, so the bitfield it tests is CUnit's own at +0x148.
struct SBitfield148Owner { int __thiscall TestBit(int bitIndex); };

int CUnit::SubmitToPlayerTable(unsigned int playerIndex, int flags, int extra)
{
    SPlayerTable* table = (SPlayerTable*)g_unk6FAB65F4;
    if (table == 0 || table->m_field3E0 == 0)
        return 0;

    //  Two loads, not one: the registry entry's +0x10 holds a pointer to
    //  the pointer this actually tests.
    void* subject = *((void***)GetThreadLocalSlot(kThreadLocalGameData))[4];

    //  A conditional expression rather than an if: the shipped code sets
    //  eax on one arm and jumps over an `xor eax, eax` on the other, which
    //  is what `? 1 : 0` produces and what an `int x = 0; if (...) x = 1;`
    //  does not.
    int extraBit = (subject != 0 && (*(int*)((char*)subject + 4) & 0x200) != 0) ? 1 : 0;

    //  One short-circuiting condition and not two statements: written as
    //  two `if (...) return 1;` in a row this compiler normalises the
    //  second one's result with neg/sbb/neg instead of testing and
    //  branching to the shared `mov eax, 1`.
    if (table->SubmitUnit(playerIndex, this, (extraBit != 0) | flags, extra, 0) != 0 ||
        ((SBitfield148Owner*)this)->TestBit(playerIndex) != 0)
        return 1;
    return 0;
}

//============================================================================
//  0x6F285630 - slot 98 (+0x188).
//
//  Index the world-object singleton's own array by the 16-bit index it
//  keeps at +0x28, take that entry's +0x34 and ask it whether this unit is
//  a member.  The `movzx` is the tell that the index really is a word and
//  not a dword that happens to be small.
//============================================================================
//  0x6F421E20 - the membership test.  A real body now, in
//  unitmembershipset.cpp on unitmembershiplist.cpp; unitmembership.h
//  carries the one declaration both this file and unit_playermasksearch.cpp
//  use.
#include "unitmembership.h"

//  0x6F3A1650 - already reconstructed as IndexedArrayHolder_6F3A1650::At
//  (misc_field_getters.cpp).  Re-declared rather than re-thunked, for the
//  reason item_selectionrefresh.cpp spells out: a second naked function at
//  the same address is two symbols for one function and costs the original
//  its score.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x28];
    unsigned short m_index;   // +0x28
    char pad2[0x2E];
    int  m_data[1];
    int  At(unsigned int index);
};

int CUnit::Method_0x188()
{
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    void** record = (void**)world->At(world->m_index);
    SUnitMembershipSet* set = (SUnitMembershipSet*)record[0x34 / 4];
    return set->Contains(this);
}

//============================================================================
//  0x6F285A40 - slot 97 (+0x184), ReleaseSelectionVisual.
//
//  Drop the unit's own selection bookkeeping: run the 0x6F2859F0 teardown,
//  release the handle at +0x278 unless it is already the g_unk6F932954
//  "none" sentinel and put the sentinel back, then clear +0x2C8 and
//  CSelectable::m_pSelectionVisual at +0x50.
//
//  Both zero stores come from the same register, which is what one
//  `xor eax, eax` feeding two stores looks like and what two separate
//  literal zeroes do not produce.
//============================================================================
//  0x6F2859F0 (`retn 0`) - CUnit's own, one step further down.
//  0x6F00D970 - already in the repo as AttachmentRelease (configgates.cpp).
//  Re-declared under that name rather than a fresh one: a second naked
//  function at the same address is two symbols for one function and costs
//  the original its score, and the canonicaliser compares symbol names.
void __fastcall AttachmentRelease(unsigned int slot);

extern const unsigned int g_unk6F932954;

void CUnit::ReleaseSelectionVisual()
{
    TearDownSelectionVisual();
    if (m_handle278 != g_unk6F932954)
    {
        AttachmentRelease(m_handle278);
        m_handle278 = g_unk6F932954;
    }
    //  +0x2C8 before +0x50, which is the shipped order and not ascending.
    m_int2C8 = 0;
    m_pSelectionVisual = 0;
}
