//============================================================================
//  NTempest::CDynTable<T> - 31 template instantiations over element
//  types this repo has not otherwise built (RTTI-confirmed via
//  agent_worktrees/classes/ after the 2026-09-07 dump_agent_worktrees.py
//  namespace/template regex fix - see docs/targets/NTempestNIpse_BaseChain.md).
//  Every one of these is the *identical* shape already established for
//  NTempest::CMemBlock's own scalar deleting destructor (Agent/cmemblock.h/
//  cmemblock_dtor.cpp): call the already-reconstructed ReleaseCMemBlockBase
//  (Pathfinding/cellbuffer.h, a naked redirect to 0x6F4B3770 - stamps
//  CMemBlock's own vtable, frees the block, restamps CEntity's), then
//  free `this` when bit 0 of `flags` is set AND `this` is non-null. No
//  per-element cleanup: CDynTable<T> is a raw byte block for a trivially-
//  destructible T, so there is nothing beyond the base free to do -
//  confirmed by the identical instruction count and shape across every one
//  of these 31, regardless of T. Grouped into one TU (Misc/'s own
//  "cross-cutting sweeps" role, src/README.md) since these are independent
//  leaves scattered across dozens of unrelated shipped modules with no
//  caller/callee relationship to each other - nothing risks being
//  incorrectly inlined by sharing a file.
//============================================================================
#include "cellbuffer.h"
#include "storm.h"

namespace NTempest {

struct CDynTable_CPaCell
{
    //  0x6F2E2870 - vtable slot 0 of NTempest::CDynTable<NIpse::CPaCell> (0x6F933210).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSortedUnit
{
    //  0x6F3797E0 - vtable slot 0 of NTempest::CDynTable<CSortedUnit> (0x6F9409D8).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_C3uVector
{
    //  0x6F38F600 - vtable slot 0 of NTempest::CDynTable<NTempest::C3uVector> (0x6F9414D4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_C2Vector
{
    //  0x6F38F630 - vtable slot 0 of NTempest::CDynTable<NTempest::C2Vector> (0x6F9414DC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_SpriteHit
{
    //  0x6F38F9A0 - vtable slot 0 of NTempest::CDynTable<SpriteHit> (0x6F9414E4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_SpriteHitWar3
{
    //  0x6F38F9D0 - vtable slot 0 of NTempest::CDynTable<SpriteHitWar3> (0x6F9414EC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSiRequest_CAgentTimer_ulongPtr
{
    //  0x6F469E70 - vtable slot 0 of NTempest::CDynTable<NIpse::CSiRequest<CAgentTimer,ulong> *> (0x6F951834).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPrRgEntry
{
    //  0x6F46A580 - vtable slot 0 of NTempest::CDynTable<NTempest::CPrRgEntry> (0x6F9517D8).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPmQueryRec
{
    //  0x6F46A5B0 - vtable slot 0 of NTempest::CDynTable<NIpse::CPmQueryRec> (0x6F9517E0).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_C2uVector
{
    //  0x6F474130 - vtable slot 0 of NTempest::CDynTable<NTempest::C2uVector> (0x6F951BFC).
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F505C20 - this class's own vtable slot 1 (the only one of the
    //  84-address batch this file/ntempest_memblockt_scalardtors.cpp cover
    //  with a second slot at all). NOT the same shape as the destructor
    //  above: a real `__except_handler4`-shaped SEH frame (cookie XOR,
    //  `__CxxFrameHandler3`, an out-of-line catch funclet) around real
    //  work - conditionally calls `sub_6F4C40A0` (a still-undumped-context
    //  callee, `this+8` as the gate), then removes `this` from a
    //  doubly-linked `{prev @ +0, next @ +4}` list it sits on and zeroes
    //  both fields - the same splice shape
    //  `CAgentBaseAbsSlot3Result::FixUp_6F4A45B0`'s own header comment
    //  documents for `SRelationLink::InsertAfter`, run in reverse (removal,
    //  not insertion). Left as a naked, real-signature thunk rather than
    //  reconstructed - the unreproducible SEH frame is the same shape
    //  docs/msvc-vc8-idioms.md already documents for several unrelated
    //  functions in this binary - so the outer name is descriptive of what
    //  it does, not a fully proven identity.
    void TeardownEntry();
};

__declspec(naked) void CDynTable_C2uVector::TeardownEntry()
{
    __asm
    {
        mov     eax, 06F505C20h
        jmp     eax
    }
}

struct CDynTable_CPmRegionPtr
{
    //  0x6F47CE90 - vtable slot 0 of NTempest::CDynTable<NIpse::CPmRegion *> (0x6F951FF4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPrRelationPtr
{
    //  0x6F480120 - vtable slot 0 of NTempest::CDynTable<NIpse::CPrRelation *> (0x6F952060).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CLrCellNode
{
    //  0x6F4849A0 - vtable slot 0 of NTempest::CDynTable<NIpse::CLrCellNode> (0x6F952174).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPaWarp
{
    //  0x6F4849D0 - vtable slot 0 of NTempest::CDynTable<NIpse::CPaWarp> (0x6F95217C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CClMember
{
    //  0x6F484A00 - vtable slot 0 of NTempest::CDynTable<NIpse::CClMember> (0x6F952184).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPresenceData
{
    //  0x6F484A30 - vtable slot 0 of NTempest::CDynTable<NTempest::CPresenceData> (0x6F95218C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CLrOpenNode
{
    //  0x6F486750 - vtable slot 0 of NTempest::CDynTable<NIpse::CLrOpenNode> (0x6F952194).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSiRequest_CPrRelation_longPtr
{
    //  0x6F4869B0 - vtable slot 0 of NTempest::CDynTable<NIpse::CSiRequest<NIpse::CPrRelation,long> *> (0x6F95219C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSiRequest_CPrBehavior_longPtr
{
    //  0x6F4869E0 - vtable slot 0 of NTempest::CDynTable<NIpse::CSiRequest<NIpse::CPrBehavior,long> *> (0x6F9521A4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSiRequest_CProximityMap_ulongPtr
{
    //  0x6F486A10 - vtable slot 0 of NTempest::CDynTable<NIpse::CSiRequest<NIpse::CProximityMap,ulong> *> (0x6F9521AC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CTargetPrTg
{
    //  0x6F48BF70 - vtable slot 0 of NTempest::CDynTable<NTempest::CTargetPrTg> (0x6F952D84).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CBhPoBehaviorPtr
{
    //  0x6F48BFA0 - vtable slot 0 of NTempest::CDynTable<NIpse::CBhPoBehavior *> (0x6F952D8C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPmCell
{
    //  0x6F49EC00 - vtable slot 0 of NTempest::CDynTable<NIpse::CPmCell> (0x6F95346C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_ulong
{
    //  0x6F49EC30 - vtable slot 0 of NTempest::CDynTable<ulong> (0x6F953474).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPmToken
{
    //  0x6F49EC60 - vtable slot 0 of NTempest::CDynTable<NIpse::CPmToken> (0x6F95347C).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CPoPosClPtr
{
    //  0x6F4A0B70 - vtable slot 0 of NTempest::CDynTable<NIpse::CPoPosCl *> (0x6F953594).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_StringElement
{
    //  0x6F4B75E0 - vtable slot 0 of NTempest::CDynTable<StringElement> (0x6F953D48).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CSortableTagRecord
{
    //  0x6F4E5320 - vtable slot 0 of NTempest::CDynTable<CSortableTagRecord> (0x6F9550D8).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_ScoreRecord
{
    //  0x6F590970 - vtable slot 0 of NTempest::CDynTable<ScoreRecord> (0x6F965764).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_TerrainLayerPriority
{
    //  0x6F7465F0 - vtable slot 0 of NTempest::CDynTable<TerrainLayerPriority> (0x6F981AC0).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CDynTable_CGxuLightPtr
{
    //  0x6F7B4AE0 - vtable slot 0 of NTempest::CDynTable<CGxuLight *> (0x6F985B84).
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CDynTable_CPaCell::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSortedUnit::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_C3uVector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_C2Vector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_SpriteHit::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_SpriteHitWar3::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSiRequest_CAgentTimer_ulongPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPrRgEntry::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPmQueryRec::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_C2uVector::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPmRegionPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPrRelationPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CLrCellNode::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPaWarp::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CClMember::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPresenceData::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CLrOpenNode::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSiRequest_CPrRelation_longPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSiRequest_CPrBehavior_longPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSiRequest_CProximityMap_ulongPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CTargetPrTg::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CBhPoBehaviorPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPmCell::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_ulong::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPmToken::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CPoPosClPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_StringElement::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CSortableTagRecord::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_ScoreRecord::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_TerrainLayerPriority::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CDynTable_CGxuLightPtr::ScalarDeletingDestructor(unsigned int flags)
{
    ReleaseCMemBlockBase(this);

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
