//============================================================================
//  0x6F4A50F0 / 0x6F4A5120 / 0x6F4A5150 / 0x6F4A51E0 / 0x6F4A5250 /
//  0x6F4A5280 / 0x6F4A52B0 / 0x6F4A53B0 / 0x6F4A53E0 / 0x6F4A57A0 -
//  `NIpse::CRlAgent`'s per-index accessors into `m_relatedAgents`
//  (`this+0x64` data pointer, `this+0x74` count - see agentbaseabs_
//  related.h for the offset evidence). Every one of them shares the same
//  opening shape: bounds-check `index` against the count, take the
//  *address* of `data[index]`, bail if what is stored there is null.
//  Written to keep that slot address alive (rather than caching the
//  dereferenced pointer in a local) because the shipped code re-derefs it
//  a second time right before the call instead of reusing a register -
//  `mov ecx,[eax] ; call` after the `lea`/`jz` pair, not `mov ecx,edx`.
//
//  CORRECTION (2026-09-07, vtable-derived): these used to be scoped under a
//  `CAgentBaseAbsRelated` struct, on the assumption that being reached
//  through CAgentBaseAbs's own vtable slots meant they were CAgentBaseAbs's
//  own methods. None of these are vtable slots at all - they're plain
//  non-virtual helpers reached only through DispatchOrder's switch
//  (Agent/agentbaseabsorderdispatch.cpp) - and every field they touch
//  belongs to `NIpse::CRlAgent` (Pathfinding/crlagent.h), which is where
//  they're declared now.
//============================================================================
#include "crlagent.h"
#include "agentbaseabs_related.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4A50F0 / 0x6F4A5120 / 0x6F4A5150
//----------------------------------------------------------------------------
void CRlAgent::AttachKind1(unsigned int index)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    (*slot)->AttachEndpointA(this, (SRelationLink*)((char*)this + 0x88));
}

void CRlAgent::AttachKind2(unsigned int index)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    (*slot)->AttachEndpointA(this, (SRelationLink*)((char*)this + 0x98));
}

void CRlAgent::AttachKind3(unsigned int index)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    (*slot)->AttachEndpointA(this, (SRelationLink*)((char*)this + 0xA8));
}

//----------------------------------------------------------------------------
//  0x6F4A51E0 - kind 0 inlined against `+0x78`; kinds 1-3 tail-call the
//  three methods above.
//----------------------------------------------------------------------------
void CRlAgent::Attach(unsigned int index, unsigned int kind)
{
    switch (kind)
    {
    case 0:
    {
        if (index >= *(unsigned int*)((char*)this + 0x74))
            return;
        CAgentRelation** slot =
            *(CAgentRelation***)((char*)this + 0x64) + index;
        if (*slot == 0)
            return;
        (*slot)->AttachEndpointA(this, (SRelationLink*)((char*)this + 0x78));
        return;
    }
    case 1:
        AttachKind1(index);
        return;
    case 2:
        AttachKind2(index);
        return;
    case 3:
        AttachKind3(index);
        return;
    }
}

//----------------------------------------------------------------------------
//  0x6F4A5250
//----------------------------------------------------------------------------
void CRlAgent::DetachIndex(unsigned int index, int alsoEndpointB)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    DetachRelation(*slot, alsoEndpointB);
}

//----------------------------------------------------------------------------
//  0x6F4A5280
//----------------------------------------------------------------------------
void CRlAgent::DetachIndexEndpointBIfLinked(unsigned int index)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    DetachEndpointBIfLinked(*slot);
}

//----------------------------------------------------------------------------
//  0x6F4A52B0
//----------------------------------------------------------------------------
void CRlAgent::SetEventId(unsigned int index, unsigned int value)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    (*slot)->m_eventId = value;
}

//----------------------------------------------------------------------------
//  0x6F4A53B0 / 0x6F4A53E0
//----------------------------------------------------------------------------
void CRlAgent::SetField2(unsigned int index, void* value)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    SRelationLink* field = (SRelationLink*)((char*)value + 0x50);
    (*slot)->AttachEndpointB(value, field);
}

void CRlAgent::SetField3(unsigned int index, void* value)
{
    if (index >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot = *(CAgentRelation***)((char*)this + 0x64) + index;
    if (*slot == 0)
        return;
    SRelationLink* field = (SRelationLink*)((char*)value + 0x60);
    (*slot)->AttachEndpointB(value, field);
}

//----------------------------------------------------------------------------
//  0x6F4A57A0
//----------------------------------------------------------------------------
void CRlAgent::SetField(unsigned int targetIndex,
                        unsigned int linkIndex, unsigned int kind)
{
    if (linkIndex >= *(unsigned int*)((char*)this + 0x74))
        return;
    CAgentRelation** slot =
        *(CAgentRelation***)((char*)this + 0x64) + linkIndex;
    if (*slot == 0)
        return;

    if (kind == 2)
        SetField2(targetIndex, *slot);
    else if (kind == 3)
        SetField3(targetIndex, *slot);
}

}  // namespace NIpse
