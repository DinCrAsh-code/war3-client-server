//============================================================================
//  CUpgradeDatabase - a CWar3SlkDatabase-derived sibling of CUnitDatabase
//  (Unit/unitdatabase.h), CAbilityDatabase (Item/abilitydatabase.h) and
//  CItemDatabase (Item/itemdatabase.h) in the checksum-provider registry
//  family (docs/notes/checksum-provider-registry.md).  Since 2026-09-13 a
//  real public `CWar3SlkDatabase -> CSynchronousData` derivation
//  (`agent_worktrees/classes/0x6F92C70C.json`'s own `hierarchy_raw`:
//  "CUpgradeDatabase: CWar3SlkDatabase, CSynchronousData;"), not a
//  standalone struct duplicating the base's fields.
//
//  Only 3 vtable slots total:
//    slot 0 - ComputeChecksum: NOT overridden - shares the base's own
//      default (0x6F005D60, funcmap `CWar3SlkDatabase::ComputeChecksum`)
//      exactly the way `CAbilityDatabase` itself does.  Modelled via real
//      public inheritance - no override declared here at all.
//    slot 1 - DebugDumpTo: the shared `nullsub_53` (0x6F002000).  Same
//      "distinct, real, empty definition of our own" convention
//      war3slkdatabase.h/unitdatabase.h both already establish - see
//      those headers' own notes.
//    slot 2 - SetRecordSet: a real override, 0x6F252E50, confirmed by
//      reading it - unlike CUnitDatabase/CAbilityDatabase, it is NOT the
//      shared 0x6F005D30 body, though it does open by calling
//      `CWar3SlkDatabase::SetRecordSet` (0x6F005D30) directly (a real
//      `call sub_6F005D30` in the dump, now modelled as a genuine call
//      into the base class's own function rather than an inlined field
//      store - see upgradedatabase_setrecordset.cpp) before wiring up
//      its own 19 SLK fields.
//
//  **`Construct()` (0x6F25F0B0's own inlined logic) stamps the shipped
//  vtable's own literal runtime address, not a C++-constructor-emitted
//  `??_7CUpgradeDatabase@@6B@` of our own** - the same choice
//  Item/abilitydatabase.h's own header note documents for
//  `LazyInitGameDataCacheField14`'s identical "allocate raw memory, write
//  the vtable pointer and fields by hand" shape: `Construct()` is not a
//  real C++ constructor (MSVC never auto-emits a vtable store for an
//  ordinary member function), and nothing in this reconstruction
//  `new`s/placement-constructs a `CUpgradeDatabase` the language's own
//  way, so there is no real constructor call site to retarget at our own
//  vtable even now that the class has real virtuals.  `check_vtables.py`
//  and `ctor_vtable_audit.py` both treat "no C++ constructor reachable
//  yet" as informational rather than a mismatch, exactly like every
//  other class in this family - see abilitydatabase.h's own note.
//
//  The real destructor (0x6F252CF0 - no dump reaches this session, so it
//  stays unidentified rather than guessed at) is out of this session's
//  scope, the same way CUnitDatabase's own much larger real dtor is (see
//  Unit/unitdatabase.h's own header note) - nothing in this
//  reconstruction ever polymorphically `delete`s a `CUpgradeDatabase`
//  either, so there is no vtable-slot gap for a scalar-deleting
//  destructor to fill here at all (unlike CUnitDatabase, this class's
//  own committed vtable is exactly 3 slots, with no 4th "DeleteSelf").
//
//  Layout: the base CWar3SlkDatabase fields (vtable pointer at +0x00,
//  m_recordSet at +0x04) followed by 15 simple field-descriptor pairs
//  (8 bytes each, +0x10..+0x84: upgradeid, race, class, flag, maxlevel,
//  used, inherit, goldbase, goldmod, lumberbase, lumbermod, timebase,
//  timemod, version, global) and then 4 indexed groups of 4
//  ("effect1".."effect4", "base1".."base4", "mod1".."mod4",
//  "code1".."code4" - each name built at runtime with Storm_578's own
//  "%s%d" format), 0x20 bytes each (+0x88, +0xA8, +0xC8, +0xE8).
//============================================================================
#ifndef UPGRADEDATABASE_H
#define UPGRADEDATABASE_H

#include "war3slkdatabase.h"

struct SUpgradeFieldPair
{
    void* m_wedbResult;
    unsigned int m_registeredValue;
};

//  One indexed group ("effectN"/"baseN"/"modN"/"codeN").  `m_lastWedbResult`
//  is a single scratch slot the loop overwrites on every iteration and
//  never reads back - the dump's own `mov [ebp-10h], eax` writes through a
//  fixed address one struct-width before the array it is filling, not a
//  per-index array of its own; only the last iteration's value survives.
struct SUpgradeIndexedGroup
{
    unsigned int m_lastWedbResult;   // scratch - see note above
    char         m_reserved[0x0C];
    unsigned int m_values[4];        // index 1..4, in order
};

class CUpgradeDatabase : public CWar3SlkDatabase
{
public:
    //  +0x00 m_vfptr, +0x04 m_recordSet, +0x08 m_haveChecksum,
    //  +0x0C m_checksum - all inherited from CWar3SlkDatabase.

    SUpgradeFieldPair m_upgradeid;    // +0x10
    SUpgradeFieldPair m_race;         // +0x18
    SUpgradeFieldPair m_class;        // +0x20
    SUpgradeFieldPair m_flag;         // +0x28
    SUpgradeFieldPair m_maxlevel;     // +0x30
    SUpgradeFieldPair m_used;         // +0x38
    SUpgradeFieldPair m_inherit;      // +0x40
    SUpgradeFieldPair m_goldbase;     // +0x48
    SUpgradeFieldPair m_goldmod;      // +0x50
    SUpgradeFieldPair m_lumberbase;   // +0x58
    SUpgradeFieldPair m_lumbermod;    // +0x60
    SUpgradeFieldPair m_timebase;     // +0x68
    SUpgradeFieldPair m_timemod;      // +0x70
    SUpgradeFieldPair m_version;      // +0x78
    SUpgradeFieldPair m_global;       // +0x80

    SUpgradeIndexedGroup m_effect;    // +0x88
    SUpgradeIndexedGroup m_base;      // +0xA8
    SUpgradeIndexedGroup m_mod;       // +0xC8
    SUpgradeIndexedGroup m_code;      // +0xE8
    //  Struct ends at +0x108.

    //  0x6F25F0B0's own inline constructor - zero the three
    //  CWar3SlkDatabase checksum-cache fields, then stamp the vtable.
    //  Unlike CItemDatabase::Construct (Item/itemdatabase_ctor.cpp), the
    //  33 SLK field-pair bytes past +0x10 are NOT zeroed here - the dump
    //  itself only ever touches +0x04/+0x08/+0x0C before the vtable
    //  stamp, a real difference from CItemDatabase's own separate,
    //  full-zeroing constructor, not an oversight.  Named `Construct` per
    //  this family's own convention; see upgradedatabase_ctor.cpp.  Not a
    //  real C++ constructor - see this header's own note above on why it
    //  stamps the shipped vtable's literal address rather than ours.
    void __thiscall Construct();

    //  0x6F252CF0 - the real (SEH-framed) ~CUpgradeDatabase(), found but
    //  not claimed by checksum-batch-A; reconstructed round2-F.  Now that
    //  this class really derives from CWar3SlkDatabase, its own tail no
    //  longer hand-stamps a flat base-view - it explicit-calls the base's
    //  own real, independently-scored ~CWar3SlkDatabase() (0x6F005D40,
    //  war3slkdatabase.cpp), matching every other reconstructed member of
    //  this family.  See upgradedatabase_destructor.cpp.
    void __thiscall DestructInternal();

    //  Vtable slot 1 (DebugDumpTo) is the shared `nullsub_53`
    //  (0x6F002000) - a distinct, real, empty definition of our own, the
    //  same convention war3slkdatabase.h/unitdatabase.h both establish.
    //  Not funcmap-registered (no dedicated address).
    virtual void DebugDumpTo(void* buffer);

    //  0x6F252E50 - vtable slot 2 override.  upgradedatabase_setrecordset.cpp.
    virtual void __thiscall SetRecordSet(void* recordSet);
};

#endif
