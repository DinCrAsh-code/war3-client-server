//============================================================================
//  0x6F54B810 - ReleaseNetEvent (formerly a naked thunk misnamed
//  "LogBadEventSlot" - it logs nothing; that guess predates this session's
//  own reading of the dump). See netdataeventqueue.h for the file-split
//  rationale.
//
//  Clears the +0x1B40 "currently latched event" cache slot if this node is
//  the one cached there, destroys the node for real (DestroyNetEvent,
//  netdataeventdestroy.cpp), then hands it back to the
//  TInstanceRecycler<CNetEvent> pool at +0x2290 through that pool's own
//  already-real CDataRecycler::ReleaseChunk (cdatarecycler_slots.cpp),
//  0x18 bytes, RTTI-tagged ".?AUCNetEvent@CNetData@@" - the exact string
//  that identified the node type in the first place.
//============================================================================
#include "netdataeventqueue.h"
#include "cdatarecycler.h"

void CNetDataEventOwner::ReleaseNetEvent(void* node)
{
    char* p = (char*)this;

    if (node == *(void**)(p + 0x1B40))
        *(void**)(p + 0x1B40) = 0;

    DestroyNetEvent(node);

    ((CDataRecycler*)(p + 0x2290))->ReleaseChunk(
        node, 0x18, ".?AUCNetEvent@CNetData@@", -2);
}
