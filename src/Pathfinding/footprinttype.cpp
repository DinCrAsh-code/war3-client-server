//============================================================================
//  0x6F32C880, 0x6F32D0B0 - the footprint type registry's two entry points.
//============================================================================
#include "footprinttype.h"

//----------------------------------------------------------------------------
//  0x6F32C880 - Ptr() first; New()'s two field stores (m_hashval, m_key)
//  inlined at the call site rather than through New() itself, which is why
//  this is not simply `m_table.New(hash, id, 0, 0)`; the shipped code adds a
//  virtual call on the freshly made node afterwards that New() does not have.
//
//  g_footprintTypeTable used to be reconstructed as a pointer
//  (`CFootprintTypeTable* g_pFootprintTypeTable`), which compiles this call
//  as a load through dword_6FAB58D4 followed by a call through whatever
//  garbage that read - the live object's own first bytes, not a pointer to
//  it. Confirmed crashing in the live game (debugger trace: faults inside
//  this call) exactly as that would predict. It is the table object itself
//  at that address - see footprinttype.h.
//----------------------------------------------------------------------------
CFootprintTypeData* __fastcall GetOrCreateFootprintTypeData(
    unsigned int id, const char* /*debugName*/)
{
    AGILE_TYPE_ID key(id);
    unsigned int hashval = key.Hash();

    CFootprintTypeData* data = g_footprintTypeTable.Ptr(hashval, key);
    if (data == 0)
    {
        data = g_footprintTypeTable.NewNode(hashval, 0, 0);
        data->m_hashval = hashval;
        data->m_key = key;
        data->FinishConstruct(id);
    }
    return data;
}
