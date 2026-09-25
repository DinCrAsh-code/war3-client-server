//============================================================================
//  0x6F48E9E0/0x6F48EA60 - WriteClusterRecord/ReadClusterRecord, one
//  SPrClusterRecord (Agent/prcluster.h) each.  Own translation unit from
//  their one caller each (SaveRecordTable/LoadRecordTable,
//  prclusterrecordtable.cpp) - both real `call`s in the shipped dump.
//============================================================================
#include "prcluster.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

//  0x6F6EFE00/0x6F6EED50 - already reconstructed (Net/cdatastore_writecfloat.cpp,
//  Net/cdatastore_readcfloat.cpp), declared locally the same way every
//  other caller in this codebase does (no shared header exists for either).
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F48E9E0 - `store` is cached once and reused for every write; none of
//  WriteDword/WriteDwordAlt2/WriteCFloat's own "chain through the return
//  value" shape shows up here (they all return the same pointer back
//  unchanged, so nothing observable differs, but the shipped body plainly
//  keeps re-reading the same register throughout rather than re-deriving
//  it from each call's own return - confirmed off the dump, not assumed).
CDataStoreScratch* __fastcall WriteClusterRecord(CDataStoreScratch* store,
                                                  const SPrClusterRecord* rec)
{
    store->WriteDword(rec->m_field00);
    store->WriteDword(rec->m_field04);
    store->WriteDwordAlt2(rec->m_field08);
    const CFloat* p0C = rec->m_pair0C;
    WriteCFloat(store, p0C);
    WriteCFloat(store, p0C + 1);
    const CFloat* p18 = rec->m_pair18;
    WriteCFloat(store, p18);
    WriteCFloat(store, p18 + 1);
    WriteCFloat(store, &rec->m_field20);
    WriteCFloat(store, &rec->m_field24);
    store->WriteDwordAlt2(rec->m_field28);
    return store;
}

//  0x6F48EA60 - Save's exact mirror (same "store cached once" shape), plus
//  the m_reserved14 reset this struct's own header comment already
//  explains.
CDataStore* __fastcall ReadClusterRecord(CDataStore* store, SPrClusterRecord* rec)
{
    store->ReadDword(&rec->m_field00);
    store->ReadDword(&rec->m_field04);
    store->ReadDwordAlt3(&rec->m_field08);
    CFloat* p0C = rec->m_pair0C;
    ReadCFloat(store, p0C);
    ReadCFloat(store, p0C + 1);
    CFloat* p18 = rec->m_pair18;
    ReadCFloat(store, p18);
    ReadCFloat(store, p18 + 1);
    ReadCFloat(store, &rec->m_field20);
    ReadCFloat(store, &rec->m_field24);
    store->ReadDwordAlt3(&rec->m_field28);
    *(unsigned int*)rec->m_reserved14 = 0;
    return store;
}
