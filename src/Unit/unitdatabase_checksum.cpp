//============================================================================
//  CUnitDatabase - provider-slots-2 batch (2026-09-04) identification: the
//  checksum-provider registry's slot 4 ("unit" -
//  docs/notes/checksum-provider-registry.md).  Found by the same RTTI walk
//  that batch already used for slots 0/2/3/5/6/16/19 - `ida_query xrefs` on
//  `CSynchronousData`'s own RTTI Base Class Descriptor
//  (`??_R1A@?0A@EA@CSynchronousData@@8`, 0x6F98DE10) lists every class
//  whose own base-class array references it; reading each hit's own
//  `??_R2<Class>@@8` array (via `ida_query bytes`) named `CUnitDatabase`
//  deriving from `CWar3SlkDatabase` deriving from `CSynchronousData` - the
//  same two-level chain `CAbilityDatabase` (slot 5) already has.
//
//  Slot assignment confirmed mechanically, not by name alone: the real,
//  unhooked registration site is `sub_6F280650` (not reconstructed this
//  session - a big SEH-framed `AcquireXProvider(int index)` helper, the
//  exact shape `GameUI/acquiregamesingleton.cpp` and
//  `Net/randdataprovider.cpp` already established for slots 6/3) -
//  `GetThreadLocalSlot(0x0D)`, `SMemAlloc(0x8FC, ...Data.h, 0x74, 0)`
//  (0x8FC being `sizeof(CUnitDatabase)`), construct via `sub_6F256610`
//  (stamps `??_7CUnitDatabase@@6B@` and zeroes/`-1`-fills its many table
//  fields - not reconstructed, out of this batch's scope), store at
//  `context->m_pCache[index]`.  Its own caller (`sub_6F2A7C30`, part of the
//  gameplay-data bring-up sequence alongside `Misc\MinUnitSpeed`-class
//  config reads) calls it with the literal index baked in at the call
//  site - `xor edx,edx ; lea ecx,[edx+4] ; call sub_6F280650` - the same
//  `AcquireRandChecksumProvider`-shaped one-line-wrapper pattern already
//  established for slot 3, just inlined into the caller here instead of
//  living in its own one-line wrapper function.  4 is exactly the "unit"
//  index `checksumproviderdebugprint.cpp`'s own six-category comment
//  already names (slots 0/1/2/3/4/6 = cheat/combat/net/rand/unit/game).
//
//  **2026-09-13 update:** now a real `virtual`, publicly inheriting
//  `CWar3SlkDatabase` (`agent_worktrees/classes/0x6F92C6A0.json`'s own
//  RTTI `hierarchy_raw`) - see unitdatabase.h's own header note for the
//  full slot-by-slot story.  Still deliberately no C++ constructor: the
//  real one (`sub_6F256610`, stamps `??_7CUnitDatabase@@6B@` and
//  zero/-1-fills the ~0x8FC-byte object) and the real destructor
//  (`sub_6F251D70`, 820 instructions, `__CxxFrameHandler3` SEH prologue)
//  are their own separate, much larger batch of work this session did not
//  take on - unitdatabase.h's own header note explains why leaving both
//  unreconstructed is still safe now that the class has real virtuals.
//  `ComputeChecksum` below is reached only by
//  `GameDataContextChecksumProviderHash`'s existing raw vtable-slot-0 call
//  through `context->m_pCache[4]` (already-committed, address-based) - so
//  nothing in this repo dispatches to it as a genuine C++ virtual call
//  either way, but the vtable this class's own destructor-forced
//  `??_7CUnitDatabase@@6B@` now emits has to have the right shape
//  regardless (`check_vtables.py` has no committed
//  `docs/targets/vtables/CUnitDatabase.txt` to compare against, the same
//  choice every sibling below CAbilityDatabase already made per
//  war3slkdatabase.h's own closing note, but `class_hierarchy_audit.py`
//  and `ctor_vtable_audit.py` both do look at this class now).
//============================================================================
//  Exactly the 4 fields `sub_6F251CD0` (`ComputeChecksum`, vtable slot 0)
//  reads - `+0x08` the have-computed-it flag, `+0x0C` the cached checksum,
//  and four `HashTaggedFieldRecordCollection`-shaped table pointers at
//  `+0x10/+0x20/+0x28/+0x30` (the gaps between them are further,
//  un-investigated per-table state `ComputeChecksum` itself never reads -
//  not invented here, per CLAUDE.md's own "only touched members get
//  names" rule).  `+0x00` is the implicit vtable pointer this class's real
//  vtable occupies in the shipped object; left as an explicit `void*`
//  here rather than a `virtual` (see the file header) since nothing in
//  this TU constructs one.
//
//  Struct moved to its own header (unitdatabase.h) once
//  unitdatabase_destructor.cpp needed the same layout for slot 3, the
//  scalar deleting destructor - see that file for slot 3's own story.
#include "unitdatabase.h"

//----------------------------------------------------------------------------
//  0x6F251CD0 - CUnitDatabase::ComputeChecksum, vtable slot 0.  Same
//  lazy-cache-then-fold shape `CAbilityDatabase::GetChecksum`
//  (Item/abilitydatabase_checksum.cpp) already established for one table;
//  this class folds four, each hashed through the already-reconstructed
//  `HashTaggedFieldRecordCollection` (Misc/genericfieldrecordhash.cpp,
//  reached here through its own `sub_6F4C9050` one-instruction alias, the
//  same indirection `abilitydatabase_checksum.cpp`'s own note explains -
//  a real `call` in the dump, kept a declared-not-defined `extern` here so
//  this build's /Ob2 cannot auto-inline it) with the same
//  rotate-left-3-then-xor combine every other checksum-provider slot 0 in
//  this file (CRandData/CGameWar3's own `ComputeChecksum`s) already uses:
//  `hash = rotl(hash,3) ^ next`, applied four times in table order,
//  followed by one more `rotl(hash,3)` after the last XOR - the dump's own
//  trailing `rol eax,3` past the final `xor eax,ebx`, not folded into the
//  loop the way a naive "rotate before combining" rewrite would.
//----------------------------------------------------------------------------
extern unsigned int __fastcall HashTaggedFieldRecordCollectionAlias(void* collection);

unsigned int CUnitDatabase::ComputeChecksum()
{
    if (m_haveChecksum != 0)
        return m_checksum;

    m_haveChecksum = 1;

    unsigned int hash = HashTaggedFieldRecordCollectionAlias(m_table10);
    hash = ((hash << 3) | (hash >> 29)) ^ HashTaggedFieldRecordCollectionAlias(m_table20);
    hash = ((hash << 3) | (hash >> 29)) ^ HashTaggedFieldRecordCollectionAlias(m_table28);
    hash = ((hash << 3) | (hash >> 29)) ^ HashTaggedFieldRecordCollectionAlias(m_table30);
    hash = (hash << 3) | (hash >> 29);

    m_checksum = hash;
    return hash;
}

//----------------------------------------------------------------------------
//  Shared `nullsub_53` (0x6F002000) - not a second reconstruction of that
//  address (it already belongs to no one, same as CWar3SlkDatabase's own
//  slot 1 - see unitdatabase.h's own header note and
//  war3slkdatabase.h's).  A distinct, real, empty definition of our own,
//  not funcmap-registered.
//----------------------------------------------------------------------------
void CUnitDatabase::DebugDumpTo(void* /*buffer*/)
{
}

//  Slot 2 is `0x6F005D30` itself - the *identical*
//  address `Item/abilitydatabase_checksum.cpp` already reconstructed as
//  `CAbilityDatabase::SetRecordSet`, not a lookalike at a different
//  address: `CWar3SlkDatabase` (the common two-level base both classes
//  share, per the RTTI chain in this file's own header note) defines this
//  slot once and neither derived class overrides it, so the shipped
//  vtables for both classes carry the same function pointer here - one
//  real reconstruction already covers both.  It is a plain `this[1] =
//  recordSet` write at a fixed offset one field past the vtable pointer,
//  which is exactly `CWar3SlkDatabase`'s own layout (both `CAbilityDatabase`
//  and, per this session's read of `sub_6F256610`'s zero-fill list two
//  fields further at `+0x40.../+0x1B4h` etc., `CUnitDatabase` keep that
//  same field live at the same offset), so calling it against a
//  `CUnitDatabase*` would be correct despite the class name on the existing
//  reconstruction - not renamed here to avoid touching a function this
//  session did not re-verify end to end.  Slot 3 (the scalar deleting
//  destructor, `sub_6F257340`) stays unreconstructed - out of this batch's
//  scope, same reasoning as the two real constructors above.
