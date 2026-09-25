//============================================================================
//  0x6F6DE4E0 - SLOTITER::Next.  Step the walk on by one node.
//
//  The precomputed `m_next` is taken when there is one; otherwise the walk
//  has run off the end of its shard and has to move the lock, which is what
//  the Leave/Enter pair inside the loop is.  The shard index is
//  `(shard + 1) % SHARDS`, and for the one-shard instantiation this
//  template is used at here that folds to a constant zero - which is why the
//  shipped stream has a bare `mov [esi+10h], 0` where a general walk would
//  have an increment and a mask.
//
//  The two `x > 0 ? x : 0` clamps are the shipped `setle`/`sub 1`/`and`
//  triple: a tail link is either a positive object pointer or a negative
//  complemented terminator address, and the walk wants null for the latter.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void SLOTITER::Next()
{
    m_current = m_next;

    if (m_current == 0)
    {
        LISTENSLOTS* owner = m_owner;
        int shard = m_shard;
        int first = shard;

        do
        {
            LeaveCriticalSection(&owner->m_lock + shard);
            m_shard = 0;
            EnterCriticalSection(&m_owner->m_lock);

            shard = m_shard;
            int tail = (&m_owner->m_list)[shard].m_terminator.m_prevlink;
            m_current = tail > 0 ? tail : 0;
            if (m_current != 0)
                break;
        }
        while (shard != first);
    }

    if (m_current != 0)
    {
        int next = ((TSLink<TCPLISTEN>*)(m_current +
                        (&m_owner->m_list)[m_shard].m_linkoffset))->m_prevlink;
        m_next = next > 0 ? next : 0;
    }
}

}  // namespace OsNet
