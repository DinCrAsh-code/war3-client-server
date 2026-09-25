//============================================================================
//  0x6F4A0A80 - NIpse::CLrPathingSys::Load, vtable slot 10.
//============================================================================
#include "pathfinesearch.h"
#include "cdatastore.h"

//  0x6F4AF090 - read one pathing-reference entry back - already
//  reconstructed as a redirected thiscall member, Pathfinding/pathref_load.cpp.
struct SPathRefReader
{
    void* ReadEntry();
};

void CPathFineSearch::Load(CDataStore* store)
{
    m_grid = (CCellGrid*)((SPathRefReader*)store)->ReadEntry();
    store->ReadWord(&m_searchTag);
}
