//============================================================================
//  0x6F49B890 - admit a client to a lane, or turn it away.
//
//  Three outcomes, and the middle one is the point of the function.  A lane
//  whose pending count has run past its limit always takes the client onto
//  its list and refuses it; a lane that already has members takes it the
//  same way unless the client is the head, in which case the head is dropped
//  and the client is let through; an empty lane lets it through untouched.
//
//  `listed` is computed before either test because the shipped code does:
//  one branchless setne off the client's own list pointer feeds all three
//  arms, which is why it is a local rather than a repeated test.
//============================================================================
#include "pathmove.h"

int SPathLane::Join(CPathClient* client)
{
    int listed = (client->m_lanePrev != 0);

    if (m_pending > m_limit)
    {
        if (!listed)
            Link(client);
        return 0;
    }

    if (m_members != 0)
    {
        if (!listed)
        {
            Link(client);
            return 0;
        }
        if (m_head != client)
            return 0;
        Unlink(client);
    }
    return 1;
}
