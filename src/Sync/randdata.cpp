//============================================================================
//  CRandData's own members.  See randdata.h for the class and
//  docs/notes/checksum-provider-registry.md for the mechanism.  Each
//  record's own seeding is SResidueKey::Set (Misc/residuekey.cpp,
//  0x6F4AB660) - a shared primitive, not reconstructed again here.
//============================================================================
#include "randdata.h"

//  Sync/ctimesynchashinto.cpp - the same per-8-byte-pair fold CTimeSync's
//  own per-tick hash already uses.  Declared with its own real parameter
//  type (CTimeSync*), not a generic void* - re-declaring it with a
//  different type would mangle to a different symbol and silently create
//  a second, unresolved declaration (CLAUDE.md's own "declaration
//  disagreeing with the real definition" link_check.py class of defect).
//  Only the pointer identity matters here, exactly as
//  ctimesynchashinto.cpp's own header comment already notes for its own
//  reason.
class CTimeSync;
void __fastcall CTimeSync_HashInto(CTimeSync* self, unsigned int* accum);

//  Storm ordinal 548 - see Misc/cheatdata.cpp for its own signature note.
int __cdecl Storm_548(char* buffer, const char* format, ...);

//----------------------------------------------------------------------------
//  0x6F280B10 - dead (zero real callers, `ida_query xrefs` came back
//  empty), but seeds all 45 records - the same count the live
//  construction path (inlined into AcquireRandDataProvider,
//  randdataprovider.cpp) uses, not an off-by-one variant of it.
//----------------------------------------------------------------------------
CRandData::CRandData()
{
    SResidueKey* record = m_history;
    for (int count = 44; count >= 0; --count)
    {
        record->Set(0);
        ++record;
    }
}

unsigned int CRandData::ComputeChecksum()
{
    unsigned int hash = 0;

    for (int i = 0; i < 45; ++i)
        CTimeSync_HashInto((CTimeSync*)&m_history[i], &hash);

    return hash;
}

void CRandData::DebugDumpTo(void* buffer)
{
    for (int i = 0; i < 45; ++i)
    {
        unsigned int value = 0;
        CTimeSync_HashInto((CTimeSync*)&m_history[i], &value);
        Storm_548((char*)buffer, "Random val: %u", value);
    }
}
