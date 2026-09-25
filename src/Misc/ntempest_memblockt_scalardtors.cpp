//============================================================================
//  NTempest::CMemBlockT<T> - 32 template instantiations over element
//  types this repo has not otherwise built (RTTI-confirmed via
//  agent_worktrees/classes/ after the 2026-09-07 dump_agent_worktrees.py
//  namespace/template regex fix - see docs/targets/NTempestNIpse_BaseChain.md).
//  Every one of these is the *identical* shape already established for
//  NTempest::CMemBlock's own scalar deleting destructor (Agent/cmemblock.h/
//  cmemblock_dtor.cpp): call the already-reconstructed ReleaseCMemBlockBase
//  (Pathfinding/cellbuffer.h, a naked redirect to 0x6F4B3770 - stamps
//  CMemBlock's own vtable, frees the block, restamps CEntity's), then
//  free `this` when bit 0 of `flags` is set AND `this` is non-null. No
//  per-element cleanup: CMemBlockT<T> is a raw byte block for a trivially-
//  destructible T, so there is nothing beyond the base free to do -
//  confirmed by the identical instruction count and shape across every one
//  of these 32, regardless of T. Grouped into one TU (Misc/'s own
//  "cross-cutting sweeps" role, src/README.md) since these are independent
//  leaves scattered across dozens of unrelated shipped modules with no
//  caller/callee relationship to each other - nothing risks being
//  incorrectly inlined by sharing a file.
//============================================================================
#include "cellbuffer.h"
#include "storm.h"

namespace NTempest {

struct CMemBlockT_CPaCell
{
    //  0x6F2DC8F0 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPaCell> (0x6F933164).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSortedUnit
{
    //  0x6F3786F0 - vtable slot 0 of NTempest::CMemBlockT<CSortedUnit> (0x6F940964).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_C3uVector
{
    //  0x6F38D6F0 - vtable slot 0 of NTempest::CMemBlockT<NTempest::C3uVector> (0x6F941460).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_C2Vector
{
    //  0x6F38D720 - vtable slot 0 of NTempest::CMemBlockT<NTempest::C2Vector> (0x6F941468).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_SpriteHit
{
    //  0x6F38D940 - vtable slot 0 of NTempest::CMemBlockT<SpriteHit> (0x6F941470).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_SpriteHitWar3
{
    //  0x6F38D970 - vtable slot 0 of NTempest::CMemBlockT<SpriteHitWar3> (0x6F941478).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSiRequest_CAgentTimer_ulongPtr
{
    //  0x6F468450 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CSiRequest<CAgentTimer,ulong> *> (0x6F95178C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPrRgEntry
{
    //  0x6F469750 - vtable slot 0 of NTempest::CMemBlockT<NTempest::CPrRgEntry> (0x6F951774).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPmQueryRec
{
    //  0x6F469780 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPmQueryRec> (0x6F95177C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_C2uVector
{
    //  0x6F473300 - vtable slot 0 of NTempest::CMemBlockT<NTempest::C2uVector> (0x6F951BF4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPmRegionPtr
{
    //  0x6F47CA00 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPmRegion *> (0x6F951F4C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPrRelationPtr
{
    //  0x6F47FF00 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPrRelation *> (0x6F951FFC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CLrCellNode
{
    //  0x6F483AA0 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CLrCellNode> (0x6F952134).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPaWarp
{
    //  0x6F483AD0 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPaWarp> (0x6F95213C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CClMember
{
    //  0x6F483B00 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CClMember> (0x6F952144).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPresenceData
{
    //  0x6F483B30 - vtable slot 0 of NTempest::CMemBlockT<NTempest::CPresenceData> (0x6F95214C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CLrOpenNode
{
    //  0x6F483FC0 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CLrOpenNode> (0x6F952154).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSiRequest_CPrRelation_longPtr
{
    //  0x6F484220 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CSiRequest<NIpse::CPrRelation,long> *> (0x6F95215C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSiRequest_CPrBehavior_longPtr
{
    //  0x6F484250 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CSiRequest<NIpse::CPrBehavior,long> *> (0x6F952164).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSiRequest_CProximityMap_ulongPtr
{
    //  0x6F484280 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CSiRequest<NIpse::CProximityMap,ulong> *> (0x6F95216C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CTargetPrTg
{
    //  0x6F48AC60 - vtable slot 0 of NTempest::CMemBlockT<NTempest::CTargetPrTg> (0x6F9529B4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CBhPoBehaviorPtr
{
    //  0x6F48AC90 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CBhPoBehavior *> (0x6F9529BC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPmCell
{
    //  0x6F49E270 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPmCell> (0x6F953454).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPmToken
{
    //  0x6F49E2A0 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPmToken> (0x6F95345C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_ulong
{
    //  0x6F49E2D0 - vtable slot 0 of NTempest::CMemBlockT<ulong> (0x6F953464).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CPoPosClPtr
{
    //  0x6F4A0780 - vtable slot 0 of NTempest::CMemBlockT<NIpse::CPoPosCl *> (0x6F95358C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_StringElement
{
    //  0x6F4B7460 - vtable slot 0 of NTempest::CMemBlockT<StringElement> (0x6F953D40).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CSortableTagRecord
{
    //  0x6F4E4CA0 - vtable slot 0 of NTempest::CMemBlockT<CSortableTagRecord> (0x6F9550CC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CParticleEmitter2_CSortableParticleRecord
{
    //  0x6F4F2420 - vtable slot 0 of NTempest::CMemBlockT<CParticleEmitter2::CSortableParticleRecord> (0x6F9552E8).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_ScoreRecord
{
    //  0x6F582920 - vtable slot 0 of NTempest::CMemBlockT<ScoreRecord> (0x6F963990).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_TerrainLayerPriority
{
    //  0x6F73EA90 - vtable slot 0 of NTempest::CMemBlockT<TerrainLayerPriority> (0x6F9819D0).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CMemBlockT_CGxuLightPtr
{
    //  0x6F7B46F0 - vtable slot 0 of NTempest::CMemBlockT<CGxuLight *> (0x6F985B7C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CMemBlockT_CPaCell::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSortedUnit::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_C3uVector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_C2Vector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_SpriteHit::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_SpriteHitWar3::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSiRequest_CAgentTimer_ulongPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPrRgEntry::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPmQueryRec::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_C2uVector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPmRegionPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPrRelationPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CLrCellNode::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPaWarp::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CClMember::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPresenceData::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CLrOpenNode::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSiRequest_CPrRelation_longPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSiRequest_CPrBehavior_longPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSiRequest_CProximityMap_ulongPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CTargetPrTg::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CBhPoBehaviorPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPmCell::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPmToken::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_ulong::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CPoPosClPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_StringElement::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CSortableTagRecord::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CParticleEmitter2_CSortableParticleRecord::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_ScoreRecord::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_TerrainLayerPriority::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CMemBlockT_CGxuLightPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
