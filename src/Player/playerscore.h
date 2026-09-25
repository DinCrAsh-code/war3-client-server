//============================================================================
//  The end-of-game score fields of a player record, and the per-type tally
//  list they are computed from.
//
//  The player record is the object `IndexedArrayHolder_6F3A1650::At`
//  (0x6F3A1650, Misc/misc_field_getters.cpp) hands back out of the world
//  object's own table - the same object `Jass/jassnatives.h` views as
//  `SJassPlayer`, `Widget/playercolor.h` as `SPlayerSlot` and
//  `Game/playerunitcounters.cpp` as `SPlayerUnitCounters`.  This is a
//  fourth view of it, for the fields `CNetSession::BuildGameResultBlob`
//  (0x6F549790, Net/netgameresultblob.cpp) serialises when a game ends -
//  one `extern` for the address, one cast per view, exactly as those three
//  already do.
//
//  Nothing here claims to know what any individual counter *means*; the
//  offsets are what the accessors read and the names say so.  What the
//  cluster as a whole is doing is not in doubt, though: every one of these
//  is read once, in order, into the GAMERESULT blob JASS's `EndGame`
//  ships, which is the score screen's own data.
//============================================================================
#ifndef PLAYERSCORE_H
#define PLAYERSCORE_H

#include "CFloat.h"
#include "itemhandleresolve.h"   // SOptionalHandleRef

//----------------------------------------------------------------------------
//  Re-declared to match Misc/misc_handle_lookups.cpp's own struct exactly -
//  same keyword, same members, same order - so MSVC mangles the calls below
//  onto that file's definition instead of inventing a second symbol the
//  linker never joins (tools/link_check.py's first defect class).
//----------------------------------------------------------------------------
struct HandleRefFieldOwner_6F473170
{
    char pad[8];
    unsigned int m_handle;   // +8
    int          m_typeTag;  // +0xC

    int  QueryField78();
    void SetField78(int value);
};

//----------------------------------------------------------------------------
//  One entry of the per-type tally list at player+0x2D0: a unit/upgrade type
//  id, two running counts and a flags byte the five walks below select on.
//  Only the four words the walks touch are named.
//----------------------------------------------------------------------------
struct SPlayerTally
{
    char          m_reserved00[0x14];
    //  +0x14 - the four-character type id, handed straight to the unit-type
    //  registry (Unit/unittypecosts.cpp) and to the upgrade table
    //  (Item/upgradeaffects.cpp).
    unsigned int  m_typeId;
    char          m_reserved18[0x1C - 0x18];
    int           m_count1C;       // +0x1C
    char          m_reserved20[0x2C - 0x20];
    int           m_count2C;       // +0x2C
    //  +0x30 - read as a *byte* by every walk (`test byte ptr [esi+30h], 8`),
    //  never as a 32-bit mask.
    unsigned char m_flags30;
};

//----------------------------------------------------------------------------
//  The list itself.  Its three words sit at +0x08 of the owning object and
//  are Storm's ordinary intrusive-list triple - link offset, terminator
//  next, terminator prevlink - but they are spelled out here rather than
//  reused from `TSExplicitList` (Storm/storm.h) for one measured reason:
//  every walk in this module reads the tail as a plain
//  `mov eax, [ebp+10h]` and steps with `mov eax,[ebp+8] / add eax,esi /
//  mov eax,[eax+4]`, with **no** null test anywhere.  `TSExplicitList`'s
//  own `Link()`/`Tail()`/`Prev()` accessors carry one (and are written the
//  way they are because the hash-table walks need the `lea` it produces -
//  see storm.h's own note), so going through them would put an extra test
//  in every one of these six loops.
//----------------------------------------------------------------------------
struct SPlayerTallyLinks
{
    int m_linkoffset;   // +0x00
    int m_next;         // +0x04
    int m_prevlink;     // +0x08  - the tail, as the field stands
};

class CPlayerTallyList
{
public:
    //  0x6F3F6590 - the total cost of everything tallied under either of
    //  the two 0x0C flag bits: for each entry, the type's own two cost
    //  fields added together and multiplied by the entry's two counts.
    int TotalTalliedCost();
    //  0x6F3F6600 - how many of the flag-4 entries whose type has any cost
    //  at all, counted through m_count1C.
    int CountFlag4WithCost();
    //  0x6F3F66A0 - the flag-blind ratio: the m_count1C total over every
    //  entry the upgrade table says `upgradeId` affects, divided by how
    //  many such types there are and scaled by g_unk6FAAE574.
    //
    //  Returned **by value**, not written through an out-parameter: `this`
    //  is in ecx, the return buffer is pushed last (arg_0) and the one real
    //  argument sits above it, which is the thiscall class-return shape
    //  docs/msvc-vc8-idioms.md's own convention table names, and `retn 8`
    //  is what it costs.
    CFloat UpgradeRatio(unsigned int upgradeId);

    //  0x6F3F6760 - PlayerRecordChecksumHash's own first checksum
    //  contribution (Player/playerrecordchecksumhash.cpp).  Folds every
    //  entry of *two* tail-linked lists this class carries: the one at
    //  +0x08 (m_entries, the same one Tail()/Prev() above already walk)
    //  through a per-entry leaf that reads offsets +0x18..+0x30
    //  (PlayerTallyEntryChecksumFields, playertallyentryhash.cpp - fields
    //  no score-screen walk in this file ever reads), and a *second*,
    //  identically-shaped list header this class also carries at +0x30
    //  (read through raw offset casts in ComputeChecksum's own .cpp
    //  rather than as a named member here - see that file) whose own
    //  entries contribute only their own +0x18 field, unfolded.
    unsigned int ComputeChecksum() const;

    //  Walking a list from the tail is the whole of Storm's encoding: a
    //  positive m_prevlink is the predecessor's *object* pointer and a
    //  negative one is the complement of the terminator's address, so one
    //  sign test both ends the walk and says "empty".
    //
    //  Tail() hands back a **pointer** and the walks test `(int)entry > 0`,
    //  which is what keeps the shipped `setle` / `sub 1` / `and` mask alive:
    //  written as an `int`, MSVC proves `(p > 0 ? p : 0) > 0` is just
    //  `p > 0` and deletes all four instructions.  Same reasoning
    //  Containers/tshash.inl's own walks are written with.
    SPlayerTally* Tail() const
    {
        int prevlink = m_entries.m_prevlink;
        return prevlink > 0 ? (SPlayerTally*)prevlink : 0;
    }
    //  Raw, like every other in-walk read of the field: the value is only
    //  ever used after its sign has been tested and the shipped code does
    //  not mask it here.
    //  The shipped walks re-read the link offset every iteration where this
    //  build hoists it, which is one of the three habits
    //  docs/msvc-vc8-idioms.md records as "where this toolchain optimises
    //  harder than the shipping one".  A `const volatile` view of the field
    //  does force the reload back - measured - but it costs more than it
    //  buys: the address then folds into one `mov [reg+reg+4]` where the
    //  shipped code has a separate `add`, and CPlayerTallyList::UpgradeRatio
    //  goes 0.512 -> 0.320 with it.  Left alone deliberately.
    SPlayerTally* Prev(SPlayerTally* entry) const
    {
        return *(SPlayerTally**)((char*)entry + m_entries.m_linkoffset + 4);
    }

    //  0x6F403890 / 0x6F403770 / 0x6F403810 - the three creation-time
    //  counter bumps `SPlayerScore::TallyOnUnitCreatedN` (below) each hand
    //  a `(typeId, flag)` pair.  Each finds or creates the tally entry for
    //  `typeId` (a call into `sub_6F4036B0`, itself a six-level closure of
    //  TSHash-shaped find/insert helpers - `sub_6F4C8520`'s hash, a
    //  `sub_6F3F7110` lookup, a `sub_6F4019F0` insert, and three more
    //  classification leaves under it), then bumps one or two of that
    //  entry's own counters - a different pair for each of the three - and
    //  conditionally republishes the change onto the caller's own object
    //  through the flag byte the found entry carries.
    //
    //  That whole closure is outside this target's dump - nothing in
    //  `asm/sub_6F3C5230_0x6F3C5230_calltree_asm.md` names any address past
    //  these three, and the six-deep chain underneath is a module of its
    //  own (unit-type hashing, not unit creation).  Kept as redirects, not
    //  guessed: each `retn 8` is read off its own `ida_query calltree_asm`
    //  answer, not inferred from this declaration - thiscall, two stack
    //  ints, matching what `TallyOnUnitCreatedN` below pushes.
    void TallyCounterAdjustA(unsigned int typeId, int flag);   // 0x6F403890
    void TallyCounterAdjustB(unsigned int typeId, int flag);   // 0x6F403770
    void TallyCounterAdjustC(unsigned int typeId, int flag);   // 0x6F403810

    char              m_reserved00[0x08];
    SPlayerTallyLinks m_entries;     // +0x08
};

//----------------------------------------------------------------------------
//  The player record, viewed for the score fields.
//----------------------------------------------------------------------------
struct SPlayerScore
{
    //  0x6F40F6F0 / 0x6F40F770 / 0x6F40F710 - three published counters read
    //  through their handle pair and divided by ten.  The division is
    //  unsigned (`mul 0CCCCCCCDh` / `shr edx,3`), which is what types the
    //  field.
    unsigned int QueryTenth130();
    unsigned int QueryTenth140();
    unsigned int QueryTenth150();
    //  0x6F40FB30 - the first two of those three, added.  Its own
    //  translation unit: the shipped body calls both for real.
    unsigned int QueryTenth130Plus140();
    //  0x6F40F760 / 0x6F40F7E0 / 0x6F40F750 / 0x6F40F7D0 - four more of the
    //  same counters, handed back undivided.  Each is `add ecx,<offset>` /
    //  `jmp` - /O2's tail call for `return m_fieldNNN.QueryField78();` with
    //  the callee in another translation unit.
    int QueryField190();
    int QueryField1A0();
    int QueryField1B0();
    int QueryField1C0();

    //  0x6F40B2B0 / 0x6F40B2C0 / 0x6F40B2D0 / 0x6F40B2E0 - four plain
    //  loads, and 0x6F40B2F0, which hands back the *address* of the
    //  sub-object at +0x2E4 rather than its contents.
    unsigned int QueryPeak24C();
    unsigned int QueryCounter250();
    unsigned int QueryCounter254();
    unsigned int QueryCounter258();
    void*        GetRecord2E4();

    //  The six that go through the tally list at +0x2D0.  Three of them are
    //  bare `mov ecx,[ecx+2D0h]` / `jmp` member tail calls into
    //  CPlayerTallyList above; the other three have their walk written out,
    //  because IDA models those three walks as tail *chunks* of the
    //  forwarder rather than as functions of their own and there is
    //  therefore nothing else for them to be a call to.
    int          TotalTalliedCost();          // 0x6F40B300
    int          TotalFlag8Count1C();         // 0x6F40B230
    int          TotalFlag8Count2C();         // 0x6F40B240
    int          CountFlag4WithCost();        // 0x6F40B260
    int          TotalFlag4Count2C();         // 0x6F40B270
    CFloat       GetUpgradeRatio();           // 0x6F40B280

    //  0x6F40FE30 / 0x6F40FD30 / 0x6F40FDB0 - the unit factory
    //  (0x6F29F990, jasscreateunit.h) calls exactly one of these three,
    //  picked by which of bits 1/2/4 of its own classification byte is set,
    //  as it finishes a newly created unit.  All three are the same shape:
    //  bump one tally counter (through the CPlayerTallyList this record's
    //  own +0x2D0 points at - a different pair of counters per function,
    //  which is the only thing that tells them apart), fire a fixed-id
    //  CEvent notification, then - if the in-game UI singleton already
    //  exists - OR a dirty bit into it.  `retn 8`, thiscall, two stack ints.
    void TallyOnUnitCreated1(unsigned int typeId, int flag);   // 0x6F40FE30
    void TallyOnUnitCreated2(unsigned int typeId, int flag);   // 0x6F40FD30
    void TallyOnUnitCreated3(unsigned int typeId, int flag);   // 0x6F40FDB0

    //  0x6F40B540 - build a CEvent record on the stack (vftable
    //  ??_7CEvent@@6B@, id 0x80261, no argument) and dispatch it through
    //  vtable slot 4 - the same shape `SGenericWorldEvent::Fire`
    //  (0x6F40B5A0, Item/itemgenericevent.cpp) and `SItemDeathEvent::Fire`
    //  (0x6F2AD4D0, Item/item_valuefloor.cpp) already carry, each with its
    //  own id and argument count.  Same reason it stays a redirect: the
    //  `__except_handler4`-shaped SEH frame this toolchain cannot
    //  reproduce (docs/msvc-vc8-idioms.md).  A thiscall member with no
    //  stack arguments - `retn 0`.
    void FireTallyChangedEvent();               // 0x6F40B540

    //  Each HandleRefFieldOwner_6F473170 is sixteen bytes, so the three at
    //  +0x130/+0x140/+0x150 and the four at +0x190..+0x1C0 are back to back
    //  with no padding between them.
    char                         m_reserved000[0x130];
    HandleRefFieldOwner_6F473170 m_counter130;   // +0x130
    HandleRefFieldOwner_6F473170 m_counter140;   // +0x140
    HandleRefFieldOwner_6F473170 m_counter150;   // +0x150
    char                         m_reserved160[0x190 - 0x160];
    HandleRefFieldOwner_6F473170 m_counter190;   // +0x190
    HandleRefFieldOwner_6F473170 m_counter1A0;   // +0x1A0
    HandleRefFieldOwner_6F473170 m_counter1B0;   // +0x1B0
    HandleRefFieldOwner_6F473170 m_counter1C0;   // +0x1C0
    char                         m_reserved1D0[0x24C - 0x1D0];
    unsigned int                 m_peak24C;      // +0x24C
    unsigned int                 m_counter250;   // +0x250
    unsigned int                 m_counter254;   // +0x254
    unsigned int                 m_counter258;   // +0x258
    char                         m_reserved25C[0x260 - 0x25C];
    //  +0x260 - the id GetUpgradeRatio hands the tally list.  The upgrade
    //  table is what consumes it (Item/upgradeaffects.cpp).
    unsigned int                 m_upgradeId260;
    char                         m_reserved264[0x2D0 - 0x264];
    CPlayerTallyList*            m_pTallies;     // +0x2D0
    //  +0x2D4/+0x2D8 - a second {handle, typeTag} pair
    //  (Item/itemhandleresolve.h's SOptionalHandleRef shape), read by
    //  0x6F2E52F0 (Unit/unit_abilityhostnotify.cpp) through
    //  QueryHandleField0x54.  Not otherwise identified.
    SOptionalHandleRef           m_ref2D4;       // +0x2D4/+0x2D8
    char                         m_reserved2DC[0x2E4 - 0x2DC];
    char                         m_record2E4[4]; // +0x2E4
};

//  dword_6FAAE574 - a CFloat *object* in the same run of constants as
//  g_CFloatZero (dword_6FAAE470); the shipped code both copies its value
//  (`mov ecx, dword_6FAAE574`) and takes its address (`mov edx, offset
//  dword_6FAAE574`).  Zero in the image and filled in at start-up, like
//  every other constant in that run, so its value is not readable from the
//  file - it is the scale UpgradeRatio multiplies by and the answer it
//  gives when there is nothing to average.  Declared, never defined: it is
//  the real game's global (CLAUDE.md, link_check.py's fifth check).
extern CFloat g_unk6FAAE574;

#endif
