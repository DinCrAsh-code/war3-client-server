//============================================================================
//  0x6F49AFA0 - take one client out of a formation lane's doubly linked
//  list.
//
//  The list is threaded through the *client*, not through nodes of its own:
//  m_lanePrev at +0x8C and m_laneNext at +0x90, with -1 rather than null as
//  the end sentinel and 0 meaning "not in any lane".  That is why the two
//  end cases test against -1 and the not-linked case tests against 0.
//
//  The member count is decremented first, and reaching zero short-circuits
//  the whole unlink: an empty lane just has both ends cleared.
//============================================================================
#include "pathmove.h"

void SPathLane::Unlink(CPathClient* client)
{
    if (client == 0 || client->m_lanePrev == 0)
        return;

    this->m_members -= 1;
    if (this->m_members == 0)
    {
        client->m_lanePrev = 0;
        client->m_laneNext = 0;
        this->m_tail = 0;
        this->m_head = 0;
        return;
    }

    CPathClient* prev = client->m_lanePrev;
    if (prev == (CPathClient*)-1)
        this->m_head = client->m_laneNext;
    else
        prev->m_laneNext = client->m_laneNext;

    CPathClient* next = client->m_laneNext;
    if (next == (CPathClient*)-1)
    {
        this->m_tail = client->m_lanePrev;
        client->m_laneNext = 0;
        client->m_lanePrev = 0;
        return;
    }

    next->m_lanePrev = client->m_lanePrev;
    client->m_laneNext = 0;
    client->m_lanePrev = 0;
}
