//============================================================================
//  0x6F4984A0 - NIpse::CLrPathingAcc::Load, vtable slot 10.  Save's own
//  counterpart (pathastar_save.cpp): every `m_grids` slot read back through
//  SPathRefReader::ReadEntry (Pathfinding/pathref_load.cpp - already a real
//  redirect thunk there, redeclared locally rather than redefined a second
//  time), then the +0x30 link array (LoadLinkArray,
//  pathastar_linkarray_load.cpp), then `m_searchStamp` chained off that
//  call's own returned store.
//============================================================================
#include "pathastar.h"
#include "cdatastore.h"

//  0x6F4AF090 - already a real thunk, Pathfinding/pathref_load.cpp.  Minimal
//  local redeclaration only - see that file's own comment.
struct SPathRefReader
{
    void* ReadEntry();
};

//  0x6F497F60 - Pathfinding/pathastar_linkarray_load.cpp.
CDataStore* __fastcall LoadLinkArray(CDataStore* store, void* arrayRaw);

void CPathAStar::Load(CDataStore* store)
{
    SAStarGrid** grid = &m_grids[0];
    unsigned int count = 4;
    do
    {
        *grid = (SAStarGrid*)((SPathRefReader*)store)->ReadEntry();
        grid++;
    } while (--count);

    LoadLinkArray(store, (char*)this + 0x30)
        ->ReadDword((unsigned int*)&m_searchStamp);
}
