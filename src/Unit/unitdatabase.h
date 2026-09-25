//============================================================================
//  CUnitDatabase - the checksum-provider registry's slot 4 ("unit"), and
//  (2026-09-13, RTTI hierarchy import) a real public
//  `CWar3SlkDatabase -> CSynchronousData` derivation
//  (`agent_worktrees/classes/0x6F92C6A0.json`'s own `hierarchy_raw`:
//  "CUnitDatabase: CWar3SlkDatabase, CSynchronousData;"), not the
//  non-virtual POD struct this file used to be - see below for what
//  changed and what did not.
//
//  Four vtable slots (0x6F92C6A0), one more than the base's three:
//    slot 0 - ComputeChecksum(): a REAL override, 0x6F251CD0, NOT the
//      base's own default (0x6F005D60) - unitdatabase_checksum.cpp.
//    slot 1 - DebugDumpTo(buffer): the shared `nullsub_53` (0x6F002000),
//      same address CWar3SlkDatabase's own slot 1 already points at.
//      Modelled the same way war3slkdatabase.h's own header note
//      establishes for that class: a distinct, real, empty definition of
//      our own here rather than simply inheriting the base's compiled
//      body - this repo never lets two C++ names/levels share one
//      compiled definition when each level's own shipped vtable slot is
//      independently worth naming, and CWar3SlkDatabase's slot 1 is
//      already the address's one funcmap-eligible claim (there is none
//      to make a second time; not funcmap-registered here either, same
//      as CWar3SlkDatabase::DebugDumpTo).
//    slot 2 - SetRecordSet(recordSet): NOT overridden - shipped body is
//      the base's own 0x6F005D30 (funcmap: CAbilityDatabase::SetRecordSet,
//      Item/abilitydatabase_checksum.cpp - see war3slkdatabase.h's own
//      header note on why that legacy name is not reclaimed).  Modelled
//      via real public inheritance: this class deliberately declares no
//      `SetRecordSet` override at all, so the compiler fills this vtable
//      slot with the identical inherited function pointer the shipped
//      binary uses - exactly `unitdatabase_checksum.cpp`'s own long-
//      standing note ("calling it against a CUnitDatabase* would be
//      correct despite the class name on the existing reconstruction").
//    slot 3 - DeleteSelf(flags): a genuinely NEW virtual this class adds
//      beyond CWar3SlkDatabase's own three (the scalar-deleting
//      destructor pattern) - 0x6F257340, unitdatabase_destructor.cpp.
//
//  **CUnitDatabase's own real constructor (0x6F256610, stamps
//  `??_7CUnitDatabase@@6B@` and zero/-1-fills the ~0x8FC-byte object) and
//  real destructor (0x6F251D70, 820 instructions, `__CxxFrameHandler3`
//  SEH prologue) both stay unreconstructed, exactly as this file already
//  documented before the RTTI hierarchy landed** - a much larger batch of
//  work than this session's own scope (retrofitting real public
//  inheritance onto the four vtable slots already reconstructed here).
//  Declaring no C++ constructor at all is deliberately safe under that
//  gap: nothing in this reconstruction ever `new`s a `CUnitDatabase`
//  (the one real call site, `sub_6F280650`'s own `AcquireXProvider`
//  registration path, is itself unreconstructed - see
//  unitdatabase_checksum.cpp's own header note), so the compiler-
//  generated implicit default constructor that would stamp *our* vtable
//  is never ODR-used and never emitted; `ctor_vtable_audit.py` has
//  nothing to see either way.  `DestructInternal` stays the naked
//  `mov eax, 06F251D70h / jmp eax` redirect it already was.
//============================================================================
#ifndef UNITDATABASE_H
#define UNITDATABASE_H

#include "war3slkdatabase.h"

class CUnitDatabase : public CWar3SlkDatabase
{
public:
    void* m_table10;               // +0x10
    char m_reserved14[0x0C];
    void* m_table20;               // +0x20
    char m_reserved24[4];
    void* m_table28;               // +0x28
    char m_reserved2C[4];
    void* m_table30;               // +0x30

    //  0x6F251CD0 - vtable slot 0, a real override.  unitdatabase_checksum.cpp.
    virtual unsigned int ComputeChecksum();

    //  Shared `nullsub_53` (0x6F002000) - see this header's own note above.
    //  Distinct, real, empty definition; unitdatabase_checksum.cpp.
    virtual void DebugDumpTo(void* buffer);

    //  Vtable slot 2 (SetRecordSet) is deliberately NOT declared here -
    //  inherited unchanged from CWar3SlkDatabase.  See this header's own
    //  note above.

    //  0x6F257340 - vtable slot 3, the scalar deleting destructor, a NEW
    //  virtual beyond the base's three.  unitdatabase_destructor.cpp.
    virtual CUnitDatabase* DeleteSelf(unsigned int flags);

    //  0x6F251D70 - the real ~CUnitDatabase(), a naked redirect declared
    //  here so DeleteSelf can call it; see unitdatabase_destructor.cpp's
    //  own header note for why it stays a redirect.
    void DestructInternal();
};

#endif
