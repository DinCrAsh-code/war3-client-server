//============================================================================
//  0x6F285110 - CUnit's vtable slot 64 (+0x100), PublishPosition.  Same
//  0x6F285xxx module as slot 63 (CUnit::SubmitToPlayerTable,
//  unit_playertable.cpp) and reuses the same idioms end to end: same
//  player-slot-table early-out, the same thread-local-game-data
//  double-dereference for the "extra" flag bit, the same
//  SPlayerTable::SubmitUnit/SBitfield148Owner::TestBit short-circuit tail.
//  What's new here is a game-mode-gated extra flag bit and a branch on
//  whether the acting slot's own handle-ref field (+0xF0) is set, which
//  picks between two different SPlayerTable submit entry points - one that
//  takes an explicit player index (the same SubmitUnit slot 63 already
//  uses) and one that doesn't (sub_6F3A3830, no dump anywhere names it
//  yet - declared and thunked locally, four stack arguments read off its
//  own `retn 10h`, `this` the table).
//
//  a1/a2 are the same two stack arguments the base CWidget slot at this
//  index takes (a position submit elsewhere in this family); CUnit folds
//  a1 into the flag word instead of forwarding a position, which is
//  consistent with slot 63 submitting the unit itself rather than a
//  position.
//============================================================================
#include "unit.h"
#include "widgetregistersubmit.h"
#include "gamecontext.h"

//  dword_6FAB65F4 - the player-slot table singleton (Unit/unit_playertable.cpp,
//  GameUI/worldframelocalplayer.cpp).  funcmap.DATA global, declared only.
extern void* g_unk6FAB65F4;

//  The player-slot table's own view this file needs - +0x28 the acting
//  slot, +0x34 a second pointer (its own +0x24 byte's bit 0 gates the
//  extra flag below), +0x3E0 the same "is anything registered" pointer
//  slot 63 tests.  Re-declared locally rather than pulled from a shared
//  header, the way every other reader of this class in this repo does
//  (GameUI/worldframelocalplayer.cpp's own comment explains why): the
//  `At` body is EXACT elsewhere and a second naked redirect for it here
//  would cost that one its own score.
struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;   // +0x28
    char           m_reserved2A[0x08];
    void*          m_field34;      // +0x34
    char           m_reserved38[0x3E0 - 0x38];
    void*          m_field3E0;     // +0x3E0

    int At(unsigned int index);
};

//  0x6F473170 - Misc/queryfield.cpp; re-declared identically (same rule as
//  above).  `this` is the acting slot's own record, offset by +0xF0 at the
//  call site.
struct HandleRefFieldOwner_6F473170 { int __thiscall QueryField78(); };

//  0x6F26F9E0 - misc_trivial_getters.cpp.  `this` is the unit itself.
struct SBitfield148Owner { int __thiscall TestBit(int bitIndex); };

//  0x6F53F160 - Game/gamemode.cpp, EXACT.  Re-declared rather than
//  re-thunked (same rule).
int __fastcall IsGameModeOne();

//  0x6F3A3830 (`retn 10h`, four stack arguments) - the no-index sibling of
//  SPlayerTable::SubmitUnit (widgetregistersubmit.h, 0x6F3A38F0): same
//  shape without the leading player-index argument.  No dump anywhere
//  names it, so it stays a declared thunk rather than a guess at its body.
struct SPlayerTableNoIndexSubmit
{
    int SubmitUnitNoIndex(void* unit, int flags, int a, int b);
};

void CUnit::PublishPosition(int a1, int a2)
{
    SPlayerTable* table = (SPlayerTable*)g_unk6FAB65F4;
    if (table == 0 || table->m_field3E0 == 0)
        return;

    IndexedArrayHolder_6F3A1650* world = (IndexedArrayHolder_6F3A1650*)table;
    unsigned int index = world->m_actingSlot;
    HandleRefFieldOwner_6F473170* record =
        (HandleRefFieldOwner_6F473170*)((char*)world->At(index) + 0xF0);
    //  Reloaded a second time here, matching the shipped code's own
    //  `movzx ebx, word ptr [esi+28h]` rather than keeping the first read.
    index = world->m_actingSlot;

    //  Same double-dereference slot 63 does for its own "extra" bit -
    //  Unit/unit_playertable.cpp.
    void* subject = *((void***)GetThreadLocalSlot(kThreadLocalGameData))[4];
    int extraBit = (subject != 0 && (*(int*)((char*)subject + 4) & 0x200) != 0) ? 1 : 0;

    int flags = extraBit | a1;

    if (IsGameModeOne() != 0 && (*((char*)world->m_field34 + 0x24) & 1) != 0)
        flags |= 1;

    if (record->QueryField78() != 0)
    {
        if (((SPlayerTableNoIndexSubmit*)table)->SubmitUnitNoIndex(this, flags, a2, 0) != 0 ||
            ((SBitfield148Owner*)this)->TestBit(index) != 0)
            return;
    }
    else
    {
        if (table->SubmitUnit(index, this, flags, a2, 0) != 0 ||
            ((SBitfield148Owner*)this)->TestBit(index) != 0)
            return;
    }
}

//----------------------------------------------------------------------------
//  0x6F3A3830 - naked redirect.  `this` in ecx, four stack arguments,
//  `retn 10h` off its own dump.
//----------------------------------------------------------------------------
__declspec(naked) int SPlayerTableNoIndexSubmit::SubmitUnitNoIndex(void*, int, int, int)
{
    __asm { mov eax, 06F3A3830h }
    __asm { jmp eax }
}
