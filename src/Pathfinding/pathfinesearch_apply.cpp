//============================================================================
//  0x6F4A0710 - NIpse::CLrPathingSys::Apply, vtable slot 3.
//============================================================================
#include "pathfinesearch.h"

//  See pathclient_apply.cpp's own comment: a minimal local redeclaration of
//  Pathfinding/handletable.h's SHandleTableEntry, to avoid that header's
//  own `game.h` pull (a different, incompatible `SHandleTable` body than
//  the one pathfinesearch.h already brought in through pathmove.h).
struct SHandleRegistrationRequest;
struct SHandleTableEntry
{
    void RegisterOrReuse(SHandleRegistrationRequest* request);
};

void CPathFineSearch::Apply(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    m_grid = 0;
    m_searchTag = 0;
}
