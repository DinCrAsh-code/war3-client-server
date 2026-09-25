//============================================================================
//  The 0x804-byte sub-object CNetData::CNetData constructs at its own +0xF38
//  (sub_6F54B000) - "a TRefCnt-shaped sub-object" per docs/notes/checksum-
//  provider-registry.md's own "cnd-ctor-agent session" note, corrected by
//  cnd-ctor-agent-2 to "not actually TRefCnt: not polymorphic" - is now
//  identified for real (cnd-surround-c session): it is a
//  `TSList<CNetEventNode, 0>` (Storm/storm.h - the compile-time-link-offset
//  twin of TSExplicitList, told apart by the exact "no add, dereference the
//  tail value directly" telltale storm.h's own class comment gives, and
//  confirmed byte-for-byte: ConstructNetDataEventQueueHeader's `m_linkoffset
//  = 0` / self-looped terminator matches `TSList<T,0>::TSList()` exactly)
//  followed immediately by a 510-entry (0x7F8-byte) `int` counter table -
//  see netdataeventqueue.h and netdataeventqueueunlinkcount.cpp for what
//  indexes into it.
//
//  The queue holds `CNetData::CNetEvent` nodes (RTTI-confirmed: the pool
//  release call in netdataeventrelease.cpp tags every free with the type
//  descriptor name ".?AUCNetEvent@CNetData@@") - the shipped binary's own
//  ~40-subclass event family netevent_dispatch.cpp's own header already
//  defers reconstructing. See netdataeventqueue.h for the full split
//  rationale (one function per TU, to keep MSVC from inlining this cluster
//  into a single body the shipped dump never has).
//============================================================================
#include "netdataeventqueue.h"
#include "tsfixedlist.inl"     // TSList<T,LINKOFFSET>::UnlinkAll, out of line
#include <string.h>            // memset

//  Force TSList<CNetEventNode,0>::UnlinkAll (0x6F5460B0) to be emitted -
//  the same "name the member so MSVC emits the body" idiom
//  ostcp_sendqueue.cpp's own comment documents for TSList<OsNet::OUTPUT,0>.
template void TSList<CNetEventNode, 0>::UnlinkAll();

//  0x6F54B000 - ConstructNetDataEventQueueHeader: self-link an intrusive
//  list header at +0x04/+0x08 (the same "next = &self,
//  prevlink = ~(int)&self" sentinel idiom `TSList<T,0>::TSList()`/Reset()
//  above uses) and zero a trailing 0x7F8-byte counter table at +0x0C -
//  written against the raw offsets rather than placement-new'd as a real
//  `CNetEventQueue`, since this reconstruction predates that identification
//  and already scores real (DIFFERS - see BEHAVIOUR) unchanged.
void __fastcall ConstructNetDataEventQueueHeader(void* self)
{
    char* p = (char*)self;

    *(int*)(p + 0) = 0;
    *(int*)(p + 4) = (int)(p + 4);
    *(int*)(p + 8) = ~(int)(p + 4);

    memset(p + 0xC, 0, 0x7F8);
}

//  0x6F54B040 - the real ~TSList<CNetEventNode,0>(): UnlinkAll() out of
//  line (0x6F5460B0, above), then the terminator's own inline Unlink() -
//  exactly the two steps `TSList<T,0>::~TSList()` (Storm/storm.h) always
//  is, the same pattern Frame/framedestroyslotlayer.cpp's own
//  DestroyFrameSlot/DestroyFrameLayer already use for a different T.
void __fastcall DestroyNetDataEventQueueHeader(void* self)
{
    CNetEventQueue* queue = (CNetEventQueue*)self;
    queue->m_list.UnlinkAll();
    queue->m_list.m_terminator.Unlink();
}
