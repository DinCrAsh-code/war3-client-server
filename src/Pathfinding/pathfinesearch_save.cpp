//============================================================================
//  0x6F4A0A50 - NIpse::CLrPathingSys::Save, vtable slot 9.
//============================================================================
#include "pathfinesearch.h"
#include "cdatastorescratch.h"

//  0x6F4AEF40 - write one pathing-reference entry (or a {-1,-1} sentinel
//  when null) - already reconstructed, Pathfinding/pathref_write.cpp.
//  Forward-declared locally the same way that file's own caller
//  (pathref_save.cpp) does.
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

void CPathFineSearch::Save(CDataStoreScratch* store)
{
    WritePathRefEntry(store, m_grid);
    store->WriteWord(m_searchTag);
}
