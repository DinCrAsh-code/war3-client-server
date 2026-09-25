//============================================================================
//  0x6F5490E0 - CNetEventQueue::MoveMatchingFlagged.  See netdataeventqueue.h
//  for the file-split rationale.
//
//  Walk +0xF38's own list from the tail backward - the same raw, unmasked
//  TailLink()/m_prevlink walk Pop() (netdataeventqueuepop.cpp) already
//  uses - and for every node whose flag byte at +0x15 matches
//  `wantFlagged`, move it: UnlinkAndCount() (netdataeventqueueunlinkcount.
//  cpp) removes it from this queue and decrements its counter slot, then
//  TSList<CNetEventNode,0>::LinkToHead() (storm.h, __forceinline) splices
//  it onto `dest`'s head instead. LinkToHead's own defensive
//  `link->Unlink()` re-runs the identical if(m_next)-gated body
//  UnlinkAndCount's own Unlink() call just ran - which is why the
//  disassembly carries that shape twice back to back; the second copy is
//  always a no-op here, since UnlinkAndCount already zeroed the node's own
//  m_next/m_prevlink.
//
//  The one real call site (SNetSessionResetTarget::NetSessionFullReset_
//  0x6F54C4F0) passes a stack-local, freshly-Reset() TSList<CNetEventNode,0>
//  as `dest` and 0 for `wantFlagged` - draining every *unflagged* event out
//  to a side list it then releases one at a time (out of this session's own
//  scope - see that function's own worktree entry).
//============================================================================
#include "netdataeventqueue.h"

void CNetEventQueue::MoveMatchingFlagged(int wantFlagged, TSList<CNetEventNode, 0>* dest)
{
    void* node = (void*)m_list.m_terminator.m_prevlink;
    if ((int)node <= 0)
        return;

    wantFlagged = (wantFlagged != 0);

    do
    {
        int prevLink = *(int*)((char*)node + 4);
        void* prevNode = prevLink > 0 ? (void*)prevLink : 0;

        int flagged = (*((unsigned char*)node + 0x15) != 0);
        if (flagged == wantFlagged)
        {
            UnlinkAndCount(node);
            dest->LinkToHead((CNetEventNode*)node);
        }

        node = prevNode;
    } while (node);
}
