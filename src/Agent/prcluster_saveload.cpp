//============================================================================
//  NIpse::CPrCluster - vtable slots 9/10 (Save/Load), the cluster-D
//  follow-up session (2026-09-12).  See prcluster.h's own per-method
//  comments for the full per-slot writeup; this file carries only the
//  bodies.
//
//  Own translation unit: Save/Load's own real calls into SaveRecordTable/
//  LoadRecordTable (prclusterrecordtable.cpp) and WritePathRefEntry/
//  SPathRefReader::ReadEntry (Pathfinding/pathref_write.cpp/pathref_load.cpp)
//  must stay real calls, not inlines.
//============================================================================
#include "prcluster.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "prclusterlivelist.h"
#include "game.h"

//  0x6F6EFE00/0x6F6EED50 - already reconstructed.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
//  0x6F4AEF40 - write one pathing-reference entry, Pathfinding/pathref_write.cpp.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F4AF090 - read one element back (SPathRefReader::ReadEntry,
//  Pathfinding/pathref_load.cpp) - out of scope, redirected; a thiscall
//  member of the store with no arguments beyond `this`.
struct SPathRefReader
{
    void* ReadEntry();
};

namespace NIpse {

//  0x6F492580
void CPrCluster::Save(CDataStoreScratch* store)
{
    CDataStoreScratch* s = SaveRecordTable(store, &m_records);

    //  This first group does not chain through each call's own return
    //  value - `s` is cached once and reused, the same shape
    //  prclusterrecordio.cpp's own WriteClusterRecord already establishes.
    //  The SECOND group below (+0x5C on) chains for real (confirmed off
    //  the dump: `s` is reassigned from `eax` starting exactly there), and
    //  the three WritePathRefEntry calls at the very end go back to using
    //  the ORIGINAL `store` parameter rather than either local - all three
    //  register-caching choices are real, not stylistic.
    s->WriteDword(m_field40);
    s->WriteDword(m_field44);
    s->WriteDwordAlt2(m_field48);
    CFloat* p4C = &m_field4C;
    WriteCFloat(s, p4C);
    WriteCFloat(s, p4C + 1);
    CFloat* p54 = &m_field54;
    WriteCFloat(s, p54);
    WriteCFloat(s, p54 + 1);
    s = s->WriteDword(m_field5C);
    s = s->WriteDword(m_field60);
    s = s->WriteDword(m_field64);
    s = s->WriteDword(m_field68);
    s = s->WriteDword(m_field6C);
    CFloat* p74 = &m_field74;
    s = WriteCFloat(s, &m_field70);
    s = WriteCFloat(s, p74);
    s->WriteDwordAlt2(m_field80);

    //  +0x78 is validated first: null if the pointer is null OR if
    //  `*(ptr+0x38) == -1` - see this class's own Save doc comment.
    void* ref78 = m_field78;
    if (ref78 != 0 && *(int*)((char*)ref78 + 0x38) == -1)
        ref78 = 0;
    WritePathRefEntry(store, ref78);
    WritePathRefEntry(store, m_field7C);
    WritePathRefEntry(store, m_field3C);
}

//  0x6F492680
void CPrCluster::Load(CDataStore* store)
{
    unsigned int* p40 = &m_field40;
    CDataStore* s = LoadRecordTable(store, &m_records);

    //  Same "cached pointer, not re-derived" shape as Save's own first
    //  group and prclusterrecordio.cpp's own readers/writers: `p40` is
    //  computed once (even before the LoadRecordTable call, per the dump)
    //  and reused as `p40`/`p40+1`/`p40+2` rather than three independent
    //  `&m_fieldXX` address computations, and `s` is cached once and reused
    //  rather than re-chained through each call's own return value.
    s->ReadDword(p40);
    s->ReadDword(p40 + 1);
    s->ReadDwordAlt3(p40 + 2);
    CFloat* p4C = &m_field4C;
    ReadCFloat(s, p4C);
    ReadCFloat(s, p4C + 1);
    CFloat* p54 = &m_field54;
    ReadCFloat(s, p54);
    ReadCFloat(s, p54 + 1);
    s = s->ReadDword(&m_field5C);
    s = s->ReadDword(&m_field60);
    s = s->ReadDword(&m_field64);
    s = s->ReadDword(&m_field68);
    s = s->ReadDword(&m_field6C);
    CFloat* p74 = &m_field74;
    s = ReadCFloat(s, &m_field70);
    s = ReadCFloat(s, p74);
    s->ReadDwordAlt3(&m_field80);

    m_field78 = ((SPathRefReader*)store)->ReadEntry();
    m_field7C = ((SPathRefReader*)store)->ReadEntry();
    m_field3C = ((SPathRefReader*)store)->ReadEntry();
    m_field84 = 0;

    //  Publish `this` onto the live-instance list rooted at
    //  `g_pTimeSync + 0x3B0` (Agent/prclusterlivelist.h) - the same
    //  "Load re-registers what Save does not stream" shape this family's
    //  own CPrClusterGroup::Load already documents.
    ((SAgentLiveList*)((char*)g_pTimeSync + 0x3B0))->InsertAtHead(this);
}

}  // namespace NIpse
