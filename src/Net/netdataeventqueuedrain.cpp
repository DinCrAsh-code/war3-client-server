//============================================================================
//  0x6F54B850 - DrainNetDataEventQueue.  See netdataeventqueue.h for the
//  file-split rationale - Pop/ReleaseNetEvent are kept in their own
//  translation units specifically so this loop keeps its real calls to them
//  rather than the optimizer folding this whole cluster into one body the
//  shipped dump never has.
//
//  Pop (CNetEventQueue::Pop, explicit-node argument always null here) and
//  release (ReleaseNetEvent) every node still queued at +0xF38, until the
//  queue is empty.
//============================================================================
#include "netdataeventqueue.h"

void __fastcall DrainNetDataEventQueue(void* self)
{
    char* p = (char*)self;
    CNetEventQueue* queue = (CNetEventQueue*)(p + 0xF38);

    void* node = queue->Pop(0);
    while (node)
    {
        ((CNetDataEventOwner*)self)->ReleaseNetEvent(node);
        node = queue->Pop(0);
    }
}
