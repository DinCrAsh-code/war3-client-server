//============================================================================
//  NTempest::CPriorityQ<T,U> - 11 template instantiations (RTTI-
//  confirmed via agent_worktrees/classes/ after the 2026-09-07
//  dump_agent_worktrees.py namespace/template regex fix - see
//  docs/targets/NTempestNIpse_BaseChain.md). Same base teardown as the
//  CMemBlockT<T>/CDynTable<T> family (Misc/ntempest_memblockt_scalardtors.cpp/
//  ntempest_dyntable_scalardtors.cpp) - ReleaseCMemBlockBase then a
//  conditional Storm free - but two real differences, confirmed
//  instruction-for-instruction across every one of these 11:
//
//  1. Each one restamps ITS OWN vtable pointer first (`mov [ecx], offset
//     ??_7CPriorityQ<T,U>@@6B@`) rather than relying on
//     ReleaseCMemBlockBase's own CMemBlock-level restamp - the same "two
//     stamps to the same field, one destined to be immediately overwritten"
//     shape Pathfinding/crlagent_dtor.cpp's own StampVtable helper exists
//     for (routed through a `volatile` pointer store so dead-store
//     elimination cannot collapse or drop it - a plain compile-time-
//     constant assignment measurably does, see that file's own comment).
//  2. The free is gated on `flags & 1` alone - **no `this != 0` guard**,
//     unlike the CMemBlockT<T>/CDynTable<T> family. Confirmed absent from
//     the dump in every one of these; not a transcription oversight.
//============================================================================
#include "cellbuffer.h"
#include "storm.h"

namespace NTempest {

//  See Pathfinding/crlagent_dtor.cpp's own StampVtable for why this goes
//  through a volatile store rather than a plain literal assignment.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

struct CPriorityQ_SpriteHit_SpriteHit
{
    //  0x6F393740 - vtable slot 0 of NTempest::CPriorityQ<SpriteHit,SpriteHit> (0x6F941524).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_SpriteHitWar3_SpriteHitWar3
{
    //  0x6F393770 - vtable slot 0 of NTempest::CPriorityQ<SpriteHitWar3,SpriteHitWar3> (0x6F94152C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CSiRequest_CAgentTimer_ulongPtr_CSiRequest_CAgentTimer_ulong
{
    //  0x6F46AA00 - vtable slot 0 of NTempest::CPriorityQ<NIpse::CSiRequest<CAgentTimer,ulong> *,NIpse::CSiRequest<CAgentTimer,ulong>> (0x6F95183C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CLrOpenNode_CLrOpenNode
{
    //  0x6F487D40 - vtable slot 0 of NTempest::CPriorityQ<NIpse::CLrOpenNode,NIpse::CLrOpenNode> (0x6F9525BC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CSiRequest_CPrRelation_longPtr_CSiRequest_CPrRelation_long
{
    //  0x6F487DD0 - vtable slot 0 of NTempest::CPriorityQ<NIpse::CSiRequest<NIpse::CPrRelation,long> *,NIpse::CSiRequest<NIpse::CPrRelation,long>> (0x6F9522F4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CSiRequest_CPrBehavior_longPtr_CSiRequest_CPrBehavior_long
{
    //  0x6F487E00 - vtable slot 0 of NTempest::CPriorityQ<NIpse::CSiRequest<NIpse::CPrBehavior,long> *,NIpse::CSiRequest<NIpse::CPrBehavior,long>> (0x6F9522FC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CSiRequest_CProximityMap_ulongPtr_CSiRequest_CProximityMap_ulong
{
    //  0x6F487E30 - vtable slot 0 of NTempest::CPriorityQ<NIpse::CSiRequest<NIpse::CProximityMap,ulong> *,NIpse::CSiRequest<NIpse::CProximityMap,ulong>> (0x6F952304).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CSortableTagRecord_CSortableTagRecord
{
    //  0x6F4E5CE0 - vtable slot 0 of NTempest::CPriorityQ<CSortableTagRecord,CSortableTagRecord> (0x6F9550F8).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_ScoreRecord_ScoreRecord
{
    //  0x6F5967D0 - vtable slot 0 of NTempest::CPriorityQ<ScoreRecord,ScoreRecord> (0x6F9663EC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_TerrainLayerPriority_TerrainLayerPriority
{
    //  0x6F7495F0 - vtable slot 0 of NTempest::CPriorityQ<TerrainLayerPriority,TerrainLayerPriority> (0x6F981AD0).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPriorityQ_CGxuLightPtr_CGxuLight
{
    //  0x6F7B4B60 - vtable slot 0 of NTempest::CPriorityQ<CGxuLight *,CGxuLight> (0x6F985B8C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CPriorityQ_SpriteHit_SpriteHit::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F941524);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_SpriteHitWar3_SpriteHitWar3::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F94152C);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CSiRequest_CAgentTimer_ulongPtr_CSiRequest_CAgentTimer_ulong::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F95183C);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CLrOpenNode_CLrOpenNode::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F9525BC);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CSiRequest_CPrRelation_longPtr_CSiRequest_CPrRelation_long::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F9522F4);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CSiRequest_CPrBehavior_longPtr_CSiRequest_CPrBehavior_long::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F9522FC);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CSiRequest_CProximityMap_ulongPtr_CSiRequest_CProximityMap_ulong::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F952304);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CSortableTagRecord_CSortableTagRecord::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F9550F8);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_ScoreRecord_ScoreRecord::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F9663EC);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_TerrainLayerPriority_TerrainLayerPriority::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F981AD0);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPriorityQ_CGxuLightPtr_CGxuLight::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F985B8C);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
