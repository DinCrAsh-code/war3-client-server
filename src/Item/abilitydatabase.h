//============================================================================
//  CAbilityDatabase - the lazily-created singleton parked at
//  CGameDataCache::m_field14 (array slot 5), one of the checksum-provider
//  registry's shared lazy-singleton slots - see
//  docs/notes/checksum-provider-registry.md.  Identified from the vtable
//  stamp its own lazy-init helper writes (`??_7CAbilityDatabase@@6B@`,
//  confirmed against the real vtable - `ida_query vtable 0x6F92E0A8`,
//  committed as docs/targets/vtables/CAbilityDatabase.txt): 3 slots.
//
//  **Real public inheritance from CWar3SlkDatabase, as of 2026-09-13.**
//  `agent_worktrees/classes/0x6F92E0A8.json`'s own Class Informer-imported
//  `hierarchy` says `CAbilityDatabase: CWar3SlkDatabase, CSynchronousData;`
//  - this class was previously modelled as a flat POD struct with no real
//  C++ base at all (flagged by `tools/class_hierarchy_audit.py` as MISSING
//  CWar3SlkDatabase/CSynchronousData from the chain), from back when
//  CWar3SlkDatabase (Item/war3slkdatabase.h) did not exist yet as a
//  reconstructed class.  Now that it does, this class derives from it for
//  real, the same `mdisp 0`/shared-vtable-pointer shape war3slkdatabase.h
//  itself documents for its own base, and docs/msvc-vc8-idioms.md's "a base
//  class that already has one costs nothing to inherit from".
//
//  All three vtable slots are the base's own, UNCHANGED - this class
//  overrides nothing:
//    slot 0 - CWar3SlkDatabase::ComputeChecksum() (0x6F005D60): the lazy
//      hash-and-cache over m_recordSet.
//    slot 1 - CWar3SlkDatabase::DebugDumpTo(buffer): the shared nullsub_53
//      (0x6F002000) default, not overridden.
//    slot 2 - CWar3SlkDatabase::SetRecordSet(recordSet) (0x6F005D30): the
//      setter slot 0's own hash walks.
//  war3slkdatabase.h's own header note already covers the full history: both
//  0x6F005D60 and 0x6F005D30 were originally found and reconstructed only
//  through THIS class's own vtable (docs/targets/vtables/CAbilityDatabase.txt)
//  under the legacy names CAbilityDatabase::GetChecksum/SetRecordSet, before
//  this class's own RTTI hierarchy was imported - CAbilityDatabase never
//  actually overrode either slot, it only ever inherited the base's bodies.
//  Now that the real base exists and this class genuinely derives from it,
//  `tools/funcmap.py` maps both addresses to their real owner
//  (CWar3SlkDatabase::ComputeChecksum/SetRecordSet, war3slkdatabase.cpp) and
//  this file declares no methods of its own at all - the previous
//  GetChecksum/Method1/SetRecordSet virtuals and their bodies
//  (abilitydatabase_checksum.cpp, now removed) are gone, superseded by the
//  identical logic already sitting in the real base class.
//
//  **This class therefore declares NO members, NO virtuals and NO
//  constructor of its own** - it exists as a name (matching the shipped
//  RTTI class) with real inheritance, nothing more.  Nothing in this
//  build's own call trees ODR-uses an implicit CAbilityDatabase
//  constructor (LazyInitGameDataCacheField14, abilitydatabase_lazyinit.cpp,
//  allocates raw memory and writes the vtable pointer and three fields by
//  hand rather than placement-`new`ing one), so the compiler never
//  instantiates one and never emits a `??_7CAbilityDatabase@@6B@` of its
//  own - `tools/check_vtables.py` reports that as "not yet emitted...
//  informational" rather than a mismatch, exactly the same as every other
//  class in this family with no constructor reachable yet.  The real,
//  complete, three-slot shipped vtable this class's own committed
//  docs/targets/vtables/CAbilityDatabase.txt describes is CWar3SlkDatabase's
//  own `??_7CWar3SlkDatabase@@6B@` in every way that matters here - shared,
//  not duplicated, since this class adds nothing to it.
//
//  Layout: sizeof(CAbilityDatabase) == sizeof(CWar3SlkDatabase) == 0x10 -
//  vtable pointer at +0x00 (CSynchronousData's own, shared down the whole
//  chain), m_recordSet at +0x04, m_haveChecksum at +0x08, m_checksum at
//  +0x0C, all inherited unchanged from CWar3SlkDatabase.  This is exactly
//  the 0x10 bytes LazyInitGameDataCacheField14's own `push 10h` (the
//  Storm_401 size argument) allocates.
//============================================================================
#ifndef ABILITYDATABASE_H
#define ABILITYDATABASE_H

#include "war3slkdatabase.h"

class CAbilityDatabase : public CWar3SlkDatabase
{
    //  Deliberately empty - see this header's own note above.
};

#endif
