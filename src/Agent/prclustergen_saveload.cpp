//============================================================================
//  NIpse::CPrClusterGen - vtable slots 9/10 (Save/Load), the cluster-D
//  follow-up session (2026-09-12).  See prcluster.h's own per-method
//  comments for the full per-slot writeup; this file carries only the
//  bodies.
//============================================================================
#include "prcluster.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "game.h"

//  0x6F6EFE00/0x6F6EED50 - already reconstructed.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
//  0x6F4AEF40 - write one pathing-reference entry, Pathfinding/pathref_write.cpp.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F4AF090 - SPathRefReader::ReadEntry, Pathfinding/pathref_load.cpp -
//  out of scope, redirected; thiscall member of the store, no arguments.
struct SPathRefReader
{
    void* ReadEntry();
};

namespace NIpse {

//  0x6F48F140 - `store` is cached once and reused throughout (never
//  re-chained through a call's own return value), the same shape
//  prclusterrecordio.cpp's own WriteClusterRecord already establishes for
//  this whole family.
void CPrClusterGen::Save(CDataStoreScratch* store)
{
    store->WriteDword(m_field0DC);
    store->WriteDword(m_field0E0);
    store->WriteDwordAlt2(m_field0E4);
    CFloat* pE8 = &m_field0E8;
    WriteCFloat(store, pE8);
    WriteCFloat(store, pE8 + 1);
    WriteCFloat(store, &m_field0FC);
    store->WriteDwordAlt2(m_field100);

    //  +0xF4 validated the same way CPrCluster::Save's own +0x78 is.
    void* refF4 = m_field0F4;
    if (refF4 != 0 && *(int*)((char*)refF4 + 0x38) == -1)
        refF4 = 0;
    WritePathRefEntry(store, refF4);
    WritePathRefEntry(store, m_field0F8);

    for (unsigned int i = 0; i < 12; i++)
    {
        const SPrClusterGenRefSlot& slot = m_refSlots[i];
        store->WriteDword(slot.m_handle);
        store->WriteDword(slot.m_typeTag);
        store->WriteDwordAlt2(slot.m_field08);
        //  See Save's own doc comment: a derived boolean, not the slot's
        //  own raw field.
        store->WriteDword(m_resolved[i] == (CHandleObject*)-1 ? 1u : 0u);
    }
}

//  0x6F491FF0 - Save's mirror; see Load's own doc comment for the
//  asymmetries.
void CPrClusterGen::Load(CDataStore* store)
{
    CDataStore* s = store->ReadDword(&m_field0DC);
    s = s->ReadDword(&m_field0E0);
    s = s->ReadDwordAlt3(&m_field0E4);
    CFloat* pE8 = &m_field0E8;
    s = ReadCFloat(s, pE8);
    s = ReadCFloat(s, pE8 + 1);
    s = ReadCFloat(s, &m_field0FC);
    s->ReadDwordAlt3(&m_field100);

    m_field0F4 = ((SPathRefReader*)store)->ReadEntry();
    m_field0F8 = ((SPathRefReader*)store)->ReadEntry();
    m_field0F0 = 0;

    for (unsigned int i = 0; i < 12; i++)
    {
        SPrClusterGenRefSlot& slot = m_refSlots[i];
        CDataStore* t = store->ReadDword(&slot.m_handle);
        t = t->ReadDword(&slot.m_typeTag);
        t = t->ReadDwordAlt3(&slot.m_field08);

        unsigned int wasEmpty = 0;
        t->ReadDword(&wasEmpty);
        //  A single, shared store site for `m_resolved[i]` - the shipped
        //  body computes the value in a register on both arms of this
        //  branch and stores it once, rather than storing separately in
        //  each arm.
        CHandleObject* resolved;
        if (wasEmpty != 0)
            resolved = (CHandleObject*)-1;
        else
            resolved = LookupHandle(slot.m_handle, (int)slot.m_typeTag);
        m_resolved[i] = resolved;
    }
}

}  // namespace NIpse
