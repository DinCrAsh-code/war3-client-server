//============================================================================
//  NIpse::CPoPos - vtable slots 9 (Save) and 10 (Load). See popos.h for
//  the class-level header comment.
//============================================================================
#include "popos.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F4AEF40 - WritePathRefEntry, already reconstructed elsewhere
//  (Pathfinding/pathclient_save.cpp) as `void __fastcall
//  WritePathRefEntry(CDataStoreScratch*, const void*)`.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F4AF090 - SPathRefReader::ReadEntry, already reconstructed elsewhere
//  (Pathfinding/pathref_load.cpp) as a thiscall member with no arguments.
struct SPathRefReader
{
    void* ReadEntry();
};

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4A76A0 - vtable slot 9, Save.
//----------------------------------------------------------------------------
void CPoPos::Save(CDataStoreScratch* store)
{
    ((CRlProperty*)this)->Save(store);

    WriteCFloat(store, &m_baseX);
    WriteCFloat(store, &m_baseY);
    WriteCFloat(store, &m_velX);
    WriteCFloat(store, &m_velY);
    WriteCFloat(store, &m_fieldD);
    WriteCFloat(store, &m_fieldE);
    WriteCFloat(store, &m_fieldF);

    WritePathRefEntry(store, m_syncChannelA);
    WritePathRefEntry(store, m_syncChannelB);
}

//----------------------------------------------------------------------------
//  0x6F4A7730 - vtable slot 10, Load. Save's exact mirror image.
//----------------------------------------------------------------------------
void CPoPos::Load(CDataStore* store)
{
    ((CRlProperty*)this)->Load(store);

    ReadCFloat(store, &m_baseX);
    ReadCFloat(store, &m_baseY);
    ReadCFloat(store, &m_velX);
    ReadCFloat(store, &m_velY);
    ReadCFloat(store, &m_fieldD);
    ReadCFloat(store, &m_fieldE);
    ReadCFloat(store, &m_fieldF);

    m_syncChannelA = (CGridRegistration*)((SPathRefReader*)store)->ReadEntry();
    m_syncChannelB = (CGridRegistration*)((SPathRefReader*)store)->ReadEntry();
}

}  // namespace NIpse
