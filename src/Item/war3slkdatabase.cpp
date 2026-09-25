//============================================================================
//  CWar3SlkDatabase's own out-of-line members.  See war3slkdatabase.h for
//  the class, its RTTI, and why only the destructor below is this file's
//  own independently-scored/claimed reconstruction.
//============================================================================
#include "war3slkdatabase.h"

//  Misc/genericfieldrecordhash.cpp.  Declared, not defined, here - same
//  local-extern convention Item/abilitydatabase_checksum.cpp and
//  Unit/unitdatabase_checksum.cpp both already use for this leaf, so /Ob2
//  cannot inline away the real `call` the shipped body makes.
extern unsigned int __fastcall HashTaggedFieldRecordCollectionAlias(void* collection);

//  Misc/citemclosure_terrain_misc.cpp.  Drains the record collection's own
//  tables and releases it (a tail-call to TRefCnt::Release()) - the same
//  teardown every derived sibling's own real (unreconstructed)
//  DestructInternal calls into for this shared base field.
extern void __fastcall DatabaseRecordSet_TeardownAndRelease(void* self);

//----------------------------------------------------------------------------
//  No dedicated address - see war3slkdatabase.h's own header note.  Nothing
//  but the (compiler-generated) vtable store.
//----------------------------------------------------------------------------
CWar3SlkDatabase::CWar3SlkDatabase()
{
}

//----------------------------------------------------------------------------
//  0x6F005D60 - vtable slot 0.  funcmap.py maps this address here as of
//  2026-09-13 - see war3slkdatabase.h's own header note for the legacy
//  CAbilityDatabase::GetChecksum name it used to carry.  Lazy-compute-and-
//  cache logic: only the branch that finds a non-null m_recordSet also sets
//  the "have I computed it" flag, so a caller that asks again before one is
//  ever set keeps re-trying every time.
//----------------------------------------------------------------------------
unsigned int CWar3SlkDatabase::ComputeChecksum()
{
    if (m_haveChecksum == 0 && m_recordSet != 0)
    {
        m_haveChecksum = 1;
        unsigned int contribution = HashTaggedFieldRecordCollectionAlias(m_recordSet);
        m_checksum ^= contribution;
        m_checksum = (m_checksum << 3) | (m_checksum >> 29);
    }
    return m_checksum;
}

//----------------------------------------------------------------------------
//  Shared nullsub_53 (0x6F002000) - not overridden by the shipped class.
//  Distinct, real, empty definition - see war3slkdatabase.h's own header
//  note and synchronousdata.h's identical convention.
//----------------------------------------------------------------------------
void CWar3SlkDatabase::DebugDumpTo(void* /*buffer*/)
{
}

//----------------------------------------------------------------------------
//  0x6F005D30 - vtable slot 2, `retn 4`.  funcmap.py maps this address here
//  as of 2026-09-13 - see war3slkdatabase.h's own header note for the
//  legacy CAbilityDatabase::SetRecordSet name it used to carry.
//----------------------------------------------------------------------------
void CWar3SlkDatabase::SetRecordSet(void* recordSet)
{
    m_recordSet = recordSet;
}

//----------------------------------------------------------------------------
//  0x6F005D40 - the real ~CWar3SlkDatabase().  This file's own claimed
//  reconstruction; see war3slkdatabase.h's own header note for the xrefs
//  that confirm it (both CUnitDatabase's and CItemDatabase's own real,
//  unreconstructed destructors tail-call into this exact address as their
//  base-class dtor call).  Ordinary (not scalar-deleting - no `flags`
//  argument, `retn 0`).  `m_recordSet` is read into a local once, matching
//  the dump's own single `mov ecx, [esi+4]` reused for both the `test` and
//  the conditional `call` (no second load) - only torn down when non-null,
//  then always zeroed.
//----------------------------------------------------------------------------
CWar3SlkDatabase::~CWar3SlkDatabase()
{
    void* recordSet = m_recordSet;
    if (recordSet != 0)
        DatabaseRecordSet_TeardownAndRelease(recordSet);
    m_recordSet = 0;
}
