//============================================================================
//  0x6F49DE40 - NIpse::CLrPath::CancelAndUnregister, vtable slot 4.  Tear
//  the in-flight search all the way down (ResetSearch with every flag set,
//  ResetStampsFrom(-1) for the two stamps ResetSearch's own +0x74 clamp
//  does not reach), clear both registration pointers, park the best-node
//  pair back at their "nothing found" sentinels, then unregister and
//  notify (SHandleTableEntry::UnregisterAndNotify, 0x6F4AEF00,
//  Pathfinding/handletable.h/.cpp - already reconstructed; `arg` is
//  forwarded but never read by that function's own body either).
//============================================================================
#include "pathmove.h"

//  See pathclient_apply.cpp's own comment: a minimal local redeclaration of
//  Pathfinding/handletable.h's SHandleTableEntry, to avoid that header's
//  own `game.h` pull (a different, incompatible `SHandleTable` body than
//  the one pathmove.h already brought in).
struct SHandleTableEntry
{
    void UnregisterAndNotify(int arg);
};

void CPathClient::CancelAndUnregister(int arg)
{
    ResetSearch(-1, 1, 1, 1);
    ResetStampsFrom(-1);

    m_targetNode = 0;
    m_ignore = 0;
    m_bestTypeTag = -1;
    m_bestHandle = (unsigned int)-1;

    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}
