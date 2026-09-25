//============================================================================
//  CWar3SlkDatabase - 0x6F875B3C (??_7CWar3SlkDatabase@@6B@), the real,
//  RTTI-confirmed base every "checksum-provider registry" .slk-backed data
//  table derives from (docs/notes/checksum-provider-registry.md):
//  CAbilityDatabase, CUnitDatabase, CItemDatabase, CUpgradeDatabase and
//  CDestructableDatabase.  Confirmed the same way CSynchronousData was
//  (Game/synchronousdata.h): its own `??_R2CWar3SlkDatabase@@8` base-class
//  array names `CSynchronousData` at `mdisp 0` -
//  `agent_worktrees/classes/0x6F875B3C.json`'s own `hierarchy`, imported
//  from the Class Informer export, says exactly this
//  (`"CWar3SlkDatabase: CSynchronousData;"`).  `mdisp 0` again means the
//  base's vtable-pointer slot is shared, not a second one bolted on.
//
//  Three vtable slots - one more than CSynchronousData's own two:
//    slot 0 - ComputeChecksum(): overrides the base's always-0 default with
//      a real lazy-hash-and-cache over `m_recordSet`.
//    slot 1 - DebugDumpTo(buffer): NOT overridden - shipped body is the
//      same shared `nullsub_53` (0x6F002000) CSynchronousData's own base
//      default already points at.  Per synchronousdata.h's own convention,
//      modelled here as a distinct, real definition (its own compiled
//      address) rather than a second reconstruction filed under
//      0x6F002000 - that address already belongs to no one, and this repo
//      never lets two C++ names claim one shipped address
//      (CLAUDE.md/docs/notes/thunk-removal-pass.md's own defect class).
//    slot 2 - SetRecordSet(recordSet): a genuinely NEW virtual this class
//      adds (CSynchronousData has no third slot) - the setter for the
//      field-record-collection slot 0's own checksum hash walks.
//
//  **Slots 0 and 2's shipped bodies (0x6F005D60, 0x6F005D30) already had a
//  real, verified reconstruction in this tree - just filed under the wrong
//  class, until 2026-09-13.** Before this class's own RTTI hierarchy was
//  imported, an earlier batch found both addresses only through
//  `CAbilityDatabase`'s own vtable (docs/targets/vtables/CAbilityDatabase.txt)
//  and named them `CAbilityDatabase::GetChecksum`/`CAbilityDatabase::SetRecordSet`
//  - CAbilityDatabase never actually overrode either slot, it only ever
//  inherited this base's own bodies, exactly the shared-slot shape
//  Unit/unitdatabase_checksum.cpp's own header note already documents for
//  slot 2 across this whole family. CAbilityDatabase (Item/abilitydatabase.h)
//  now really derives from this class - `funcmap.py`'s two entries for
//  0x6F005D60/0x6F005D30 were moved onto `CWar3SlkDatabase::ComputeChecksum`/
//  `CWar3SlkDatabase::SetRecordSet` below (their real owner) in that same
//  change, and the old `CAbilityDatabase::GetChecksum`/`SetRecordSet`
//  bodies (abilitydatabase_checksum.cpp) were deleted rather than left as a
//  second, unregistered definition of the identical logic. One shipped
//  address still maps to exactly one funcmap entry - it is just this
//  class's own name now, not a legacy sibling's. This class still has no
//  committed `docs/targets/vtables/CWar3SlkDatabase.txt` of its own (see
//  this header's own closing note) - CAbilityDatabase's committed
//  `docs/targets/vtables/CAbilityDatabase.txt` is the same three slots,
//  now unowned by any override of CAbilityDatabase's own since it declares
//  none.
//
//  **Only the destructor (0x6F005D40) is this file's own claimed,
//  independently-scored reconstruction.**  It is short and SEH-free (no
//  __CxxFrameHandler3 prologue, unlike the five big derived
//  `DestructInternal`s this repo has already declined to reconstruct for
//  exactly that reason), tears down `m_recordSet` through the
//  already-reconstructed `DatabaseRecordSet_TeardownAndRelease`
//  (Misc/citemclosure_terrain_misc.cpp), and is exactly what every derived
//  class's own real destructor tail-calls into as its base-class dtor call
//  (confirmed via `worktree_store.py xrefs 0x6F005D40 --to`:
//  `CUnitDatabase::DestructInternal`/`CItemDatabase::DestructInternal`,
//  i.e. the two classes' own *real*, unreconstructed 0x6F251D70/0x6F2574C0
//  bodies, both call-and-tail-jmp into it).  A genuine, ordinary (not
//  scalar-deleting - no `flags` argument, `retn 0`) `~CWar3SlkDatabase()`,
//  matching this repo's existing `~CItemDatabase()`-shaped naming for a
//  class's own real destructor.
//
//  **Two other addresses this class's own vtable symbol string turned up
//  in a grep are NOT this class's constructor** - 0x6F2A7C30 and
//  0x6F2BAC30, both large SLK-bring-up functions (the former already named
//  and scoped out by Unit/unitdatabase_checksum.cpp's own header note: "its
//  own caller (sub_6F2A7C30), part of the gameplay-data bring-up sequence
//  ... not reconstructed this session").  Each merely placement-constructs
//  a short-lived, anonymous `CWar3SlkDatabase` object of its own (stamping
//  this class's vtable inline, zeroing the same three fields below - the
//  base ctor fully inlined away, the same fate CSynchronousData's own ctor
//  documents) and hands it to `sub_6F005D90` - which is `SStateHolder::
//  MixState` (Misc/misc_leaves_batch4.cpp), an unrelated, already-named
//  generic checksum-mixing helper, not a CWar3SlkDatabase method. Neither
//  function is a class member at all - out of scope for this batch, which
//  is this base class's own ctor/dtor/vtable only.
//
//  **No standalone constructor address exists to claim.**  The same
//  0x10-byte allocation the two functions above make (`push 10h` before
//  their own `Storm_401` call) confirms this class's own size, and their
//  own inline stamp-and-zero sequence is exactly what a real
//  `CWar3SlkDatabase()` ctor would compile to - small enough that /Ob2
//  inlines it flat into every caller, the identical fate
//  `synchronousdata.h`'s own header documents for `CSynchronousData()`
//  (0x6F001FE0) and every derived sibling's own `Construct()` here already
//  independently rediscovers ("zero the three CWar3SlkDatabase
//  checksum-cache fields, then stamp the vtable" -
//  Game/upgradedatabase.h/Unit/destructabledatabase.h). Modelled the same
//  way: a real, trivial, address-less constructor below.
//
//  This class deliberately has no `docs/targets/vtables/CWar3SlkDatabase.txt`
//  ground truth file - the same choice every derived sibling below
//  CAbilityDatabase already made (`ls docs/targets/vtables/` has no
//  CUnitDatabase.txt/CItemDatabase.txt/CUpgradeDatabase.txt/
//  CDestructableDatabase.txt either).  `check_vtables.py` only compares a
//  class against a committed file; without one it is silently skipped, and
//  that is exactly right here: this class's own real, compiled
//  `??_7CWar3SlkDatabase@@6B@` (its destructor forces the compiler to emit
//  it) legitimately names *our own* `ComputeChecksum`/`SetRecordSet`
//  symbols in slots 0/2, not `CAbilityDatabase::GetChecksum`/
//  `SetRecordSet` - correct C++, but a byte-for-byte different symbol name
//  than the legacy-named reconstruction `funcmap.py` maps those two
//  addresses to, which a committed ground-truth file would have no way to
//  express without either wrongly renaming CAbilityDatabase's own entry
//  (out of scope) or flagging a false MISMATCH.
//============================================================================
#ifndef WAR3SLKDATABASE_H
#define WAR3SLKDATABASE_H

#include "synchronousdata.h"

class CWar3SlkDatabase : public CSynchronousData
{
public:
    void*        m_recordSet;     // +0x04
    unsigned int m_haveChecksum;  // +0x08
    unsigned int m_checksum;      // +0x0C

    //  No dedicated address - see this header's own note above.  Compiles
    //  to nothing but the vtable store, exactly like CSynchronousData's own
    //  ctor.
    CWar3SlkDatabase();

    //  0x6F005D60 - vtable slot 0.  funcmap.py's own reconstruction of this
    //  address (war3slkdatabase.cpp) - see this header's own note above for
    //  the legacy CAbilityDatabase::GetChecksum name this used to be filed
    //  under, moved here 2026-09-13 once CAbilityDatabase started really
    //  deriving from this class.
    virtual unsigned int ComputeChecksum();

    //  Shared `nullsub_53` (0x6F002000) - not overridden by the shipped
    //  class.  Modelled as a distinct, real, empty definition per
    //  synchronousdata.h's own convention - see this header's own note.
    virtual void DebugDumpTo(void* buffer);

    //  0x6F005D30 - vtable slot 2, a genuinely NEW virtual (CSynchronousData
    //  has only two slots).  Same "moved here from the legacy
    //  CAbilityDatabase::SetRecordSet name" story as ComputeChecksum above.
    virtual void SetRecordSet(void* recordSet);

    //  0x6F005D40 - this file's own claimed reconstruction.  war3slkdatabase.cpp.
    ~CWar3SlkDatabase();
};

#endif
