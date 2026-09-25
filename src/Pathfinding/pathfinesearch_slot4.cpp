//============================================================================
//  0x6F4A3960 - NIpse::CLrPathingSys::CancelAndUnregister, vtable slot 4.
//============================================================================
#include "pathfinesearch.h"

//  See pathclient_apply.cpp's own comment on why this is a minimal local
//  redeclaration rather than `#include "handletable.h"`.
struct SHandleTableEntry
{
    void UnregisterAndNotify(int arg);
};

void CPathFineSearch::CancelAndUnregister(int arg)
{
    Reset(1);
    m_grid = 0;

    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}
