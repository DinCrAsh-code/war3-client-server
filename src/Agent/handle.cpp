//============================================================================
//  Global handle table lookup.
//
//  Reads g_pHandleTable through Pathfinding/handletable.h's own SHandleTable
//  (an STwoIndexedFreeLists, Math/indexedfreelist.h) rather than a
//  second, independently-modelled struct of the same name - see game.h's
//  own note at LookupHandle's declaration for why only one definition of
//  SHandleTable may exist in the tree.  The positive pool lives at `table`
//  itself (STwoIndexedFreeLists's own leading SIndexedFreeListOwner
//  shape); the negative one is the named m_negativeOwner at +0x20.  Each
//  SIndexedFreeListRecord {m_field0, m_field4} is the same two-word shape
//  as this file's own SHandleSlot {m_state, m_object}, just named for the
//  allocator's own generic use rather than this table's - reinterpreted
//  the same way agentbaseabscomputechecksum.cpp's CAgentBaseAbsRaw reuses
//  an existing object under a raw interface.
//============================================================================
#include "game.h"
#include "../Pathfinding/handletable.h"

struct SHandleSlot
{
    int             m_state;            // 0x00
    CHandleObject*  m_object;           // 0x04
};

//----------------------------------------------------------------------------
//  0x6F03FA30 - resolve a handle, but only if the object it names still has
//  the expected type tag.  The handle's top bit selects one of the two slot
//  arrays; the remaining bits are the index into it.
//----------------------------------------------------------------------------
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag)
{
    SHandleTable* table = g_pHandleTable;
    unsigned int alt = handle >> 31;

    SIndexedFreeListOwner* positive = (SIndexedFreeListOwner*)table;
    SIndexedFreeListOwner* negative = &table->m_negativeOwner;

    int state;
    if (alt == 0)
    {
        if (handle >= positive->m_capacity)
            return 0;
        state = ((SHandleSlot*)positive->m_records)[handle].m_state;
    }
    else
    {
        unsigned int index = handle & 0x7FFFFFFF;
        if (index >= negative->m_capacity)
            return 0;
        state = ((SHandleSlot*)negative->m_records)[index].m_state;
    }

    if (state != -2)
        return 0;

    CHandleObject* object = alt
        ? ((SHandleSlot*)negative->m_records)[handle & 0x7FFFFFFF].m_object
        : ((SHandleSlot*)positive->m_records)[handle].m_object;

    return object->m_typeTag == typeTag ? object : 0;
}
