//============================================================================
//  0x6F549070 - CNetEventQueue::UnlinkAndCount.  See netdataeventqueue.h for
//  the file-split rationale and netdataeventqueueheader.cpp for the +0xF38
//  sub-object's own identification.
//============================================================================
#include "netdataeventqueue.h"

//  Unlink one node (`node`, or - never actually reached from this closure's
//  own callers, which always resolve a real node first, see
//  netdataeventqueuepop.cpp - the list's own terminator when `node` is
//  null) via the ordinary TSLink<T>::Unlink() shape, then decrement the
//  counter table entry for it: index = node's own type byte at +0x14, plus
//  0xFF more (the array's own upper half) when the flag byte at +0x15 is
//  set. Returns the decremented counter's own address - real, but unused by
//  every caller in this closure (each keeps the node pointer it already
//  had instead). CNetEvent's own layout past +0x14/+0x15 is out of this
//  session's scope (the ~40-subclass family netevent_dispatch.cpp already
//  defers).
int* CNetEventQueue::UnlinkAndCount(void* node)
{
    TSLink<CNetEventNode>* link =
        node ? (TSLink<CNetEventNode>*)node : &m_list.m_terminator;
    link->Unlink();

    unsigned char type    = *((unsigned char*)node + 0x14);
    bool          flagged = (*((unsigned char*)node + 0x15) != 0);
    int index = type + (flagged ? 0xFF : 0);

    int* counter = &m_typeCounts[index];
    --*counter;
    return counter;
}
