//============================================================================
//  0x6F49B830 - put one client on the tail of a formation lane, unlinking it
//  from wherever it was first.
//
//  The unlink is unconditional: the caller does not know whether the client
//  is already listed, and PathLaneUnlink's own not-linked test makes the
//  call free when it is not.  The count is bumped *after* it, so a client
//  moving within the same lane nets out at the same count.
//============================================================================
#include "pathmove.h"

void SPathLane::Link(CPathClient* client)
{
    Unlink(client);

    this->m_members += 1;
    if (this->m_members == 1)
    {
        this->m_tail = client;
        this->m_head = client;
        client->m_lanePrev = (CPathClient*)-1;
        client->m_laneNext = (CPathClient*)-1;
        return;
    }

    client->m_laneNext = (CPathClient*)-1;
    client->m_lanePrev = this->m_tail;
    this->m_tail = client;
    client->m_lanePrev->m_laneNext = client;
}
