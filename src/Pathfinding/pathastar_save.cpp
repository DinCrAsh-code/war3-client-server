//============================================================================
//  0x6F498460 - NIpse::CLrPathingAcc::Save, vtable slot 9.  Every `m_grids`
//  pointer as a path-ref entry, then the +0x30 link array
//  (SaveLinkArray, pathastar_linkarray_save.cpp), then `m_searchStamp` as a
//  plain dword chained off that call's own returned store - the shipped
//  code re-loads `m_searchStamp` into a register before the link-array call
//  (register pressure, not a semantic reordering) but only ever writes it
//  after, which is what the chained `->WriteDword` reproduces.
//============================================================================
#include "pathastar.h"
#include "cdatastorescratch.h"

//  0x6F4AEF40 - write one pathing-reference entry, Pathfinding/pathref_write.cpp.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F497ED0 - Pathfinding/pathastar_linkarray_save.cpp.
CDataStoreScratch* __fastcall SaveLinkArray(CDataStoreScratch* store,
                                             const void* arrayRaw);

void CPathAStar::Save(CDataStoreScratch* store)
{
    SAStarGrid* const* grid = &m_grids[0];
    unsigned int count = 4;
    do
    {
        WritePathRefEntry(store, *grid);
        grid++;
    } while (--count);

    unsigned int searchStamp = m_searchStamp;
    SaveLinkArray(store, (char*)this + 0x30)->WriteDword(searchStamp);
}
