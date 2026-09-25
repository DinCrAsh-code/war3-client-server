//============================================================================
//  0x6F49A330 - NIpse::CLrPathingAcc::Apply, vtable slot 3.  Register through
//  the handle-table base the same way CRlAgent::Apply/CPathClient::Apply do
//  (Pathfinding/crlagent.h, pathclient_apply.cpp), then reset every field a
//  fresh search needs: the four grid pointers, the search stamp, and the
//  +0x30 link array grown to a flat 256 zero-filled slots.
//============================================================================
#include "pathastar.h"

//  See pathclient_apply.cpp's own comment: a minimal local redeclaration of
//  Pathfinding/handletable.h's SHandleTableEntry, to avoid that header's own
//  `game.h` pull (a different, incompatible `SHandleTable` body than the one
//  pathastar.h already brought in via pathmove.h).
struct SHandleTableEntry
{
    void RegisterOrReuse(SHandleRegistrationRequest* request);
};

void CPathAStar::Apply(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    SAStarLink fill;
    fill.m_y = 0;

    m_grids[0] = 0;
    m_grids[1] = 0;
    m_grids[2] = 0;
    m_grids[3] = 0;
    m_searchStamp = 0;

    fill.m_flags = 0;
    fill.m_x     = 0;

    ((SAStarLinkArray*)((char*)this + 0x30))->Grow(&fill, 0x100);
}
