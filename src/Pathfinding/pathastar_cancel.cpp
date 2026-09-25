//============================================================================
//  0x6F49A600 - NIpse::CLrPathingAcc::CancelAndUnregister, vtable slot 4.
//  Reset(1) (already reconstructed, pathastarreset.cpp) with `this`
//  unchanged, zero the four grid pointers again, empty the +0x30 link array
//  back to zero live elements, then unregister through the handle-table
//  base (Pathfinding/handletable.h, same shape CPathClient::
//  CancelAndUnregister already uses).
//============================================================================
#include "pathastar.h"

struct SHandleTableEntry
{
    void UnregisterAndNotify(int arg);
};

void CPathAStar::CancelAndUnregister(int arg)
{
    Reset(1);

    m_grids[0] = 0;
    m_grids[1] = 0;
    m_grids[2] = 0;
    m_grids[3] = 0;

    SAStarLinkArray* pending = (SAStarLinkArray*)((char*)this + 0x30);
    if (pending->m_count != 0)
        pending->EraseRange(0, pending->m_count);

    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}
