//============================================================================
//  Shared declarations for the +0xF38 event-queue sub-object CNetData::
//  CNetData constructs (Net/netdataeventqueueheader.cpp's own file header
//  has the full identification: a `TSList<CNetEventNode,0>` - Storm/storm.h
//  - followed by a 510-entry per-(type,flag) counter table).
//
//  Split one function per translation unit on purpose, matching CLAUDE.md's
//  "one translation unit per original module" rule applied at function
//  grain here: every one of these is small enough that compiling
//  Pop/UnlinkAndCount/DestroyNetEvent/ReleaseNetEvent into the same TU as
//  DrainNetDataEventQueue gets them inlined into its loop wholesale - which
//  the real, unhooked shipped binary does not do (its own dump keeps four
//  separate `call`s). Real modules in this shipped binary were separate
//  compilands; that boundary is what the file split reproduces here, since
//  this build has no `__declspec(noinline)` in it to fake the same effect.
//============================================================================
#ifndef NETDATAEVENTQUEUE_H
#define NETDATAEVENTQUEUE_H

//  The real, unreconstructed CNetData::CNetEvent - see netdataeventqueue
//  header.cpp's own file header. Left opaque: TSList<T,LINKOFFSET>/TSLink<T>
//  never dereference T, and this closure's own accesses go through raw
//  offsets with their own comments rather than named fields (there is no
//  reconstructed CNetEvent to add a member to).
struct CNetEventNode;

//  The +0xF38 sub-object's own real shape: TSList<CNetEventNode,0> (0xC
//  bytes) immediately followed by the 0x7F8-byte per-(type,flag) counter
//  table UnlinkAndCount decrements. A thin wrapper around the real template
//  rather than a class with its own storage semantics - the actual event
//  objects live in the TInstanceRecycler<CNetEvent> pool at CNetData+0x2290,
//  not here.
#include "storm.h"

struct CNetEventQueue
{
    TSList<CNetEventNode, 0> m_list;               // +0x00
    int                      m_typeCounts[0x1FE];  // +0x0C (510 entries)

    //  0x6F549070 - Net/netdataeventqueueunlinkcount.cpp.
    int* UnlinkAndCount(void* node);

    //  0x6F549180 - Net/netdataeventqueuepop.cpp.
    void* Pop(void* node);

    //  0x6F5490E0 - Net/netdataeventqueuemove.cpp.
    void MoveMatchingFlagged(int wantFlagged, TSList<CNetEventNode, 0>* dest);
};

//  0x6F549B80 - Net/netdataeventqueuealloc.cpp.  `pool` is the
//  TInstanceRecycler<CNetEvent>-shaped CDataRecycler sub-object at
//  CNetData+0x2290 (the same one ReleaseNetEvent, above, hands its own
//  node back to) - passed in directly by every caller
//  (LookupOrEnqueueEvent_6F54C490 does `self+0x2290`), never a
//  CNetEventQueue itself.
void* __fastcall AllocEventSlot_6F549B80(void* pool);

//  0x6F54B000 / 0x6F54B040 - Net/netdataeventqueueheader.cpp.
void __fastcall ConstructNetDataEventQueueHeader(void* self);
void __fastcall DestroyNetDataEventQueueHeader(void* self);

//  0x6F548DF0 - Net/netdataeventdestroy.cpp.
void __fastcall DestroyNetEvent(void* self);

//  0x6F54B810 - Net/netdataeventrelease.cpp.  `retn 4`: a real __thiscall
//  (self in ecx, node on the stack), not __fastcall - declared as a
//  one-argument member so the compiler puts `self` in ecx and `node` on
//  the stack the same way (the same LenPrefixedCopier::Copy idiom
//  netclient_notifyevent_0x25.cpp already documents).
struct CNetDataEventOwner
{
    void ReleaseNetEvent(void* node);
};

//  0x6F54B850 - Net/netdataeventqueuedrain.cpp.
void __fastcall DrainNetDataEventQueue(void* self);

#endif
