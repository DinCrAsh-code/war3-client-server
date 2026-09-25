//============================================================================
//  0x6F37B6D0 - TSGrowableArray<CAgentPtr>::Assign.
//
//  Drop what is there, reserve exactly `count`, placement-copy that many
//  elements out of `src` and store the new count.  The per-element `jz` is
//  placement new's own null test, not a guard the source wrote; the
//  add-ref inside it is CAgentPtr's copy constructor, written out here for
//  the same reason Containers/unitagentptrarray.cpp writes its own out -
//  declaring a copy constructor on the class collides with the converting
//  one's readable name (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: ReleaseAndReserve is a real call here.
//============================================================================
#include "tsarray.inl"
#include "agent.h"

//  Declared, not defined, here: Agent/agentptrarray_resize.cpp carries the
//  real specialization (its own header explains why it is written out
//  rather than instantiated from the generic primary template
//  Containers/tsarray.inl now carries). Without this declaration ahead of
//  the call below, this TU would see only the generic primary template and
//  implicitly instantiate its own copy - a second definition of the same
//  linker symbol, LNK2005 against agentptrarray_resize.obj.
template <>
void TSGrowableArray<CAgentPtr>::ReleaseAndReserve(unsigned int alloc);

template <>
void TSGrowableArray<CAgentPtr>::Assign(unsigned int count,
                                        const CAgentPtr* src)
{
    ReleaseAndReserve(count);

    for (unsigned int i = 0; i < count; i++)
    {
        CAgentPtr* slot = &m_data[i];
        if (slot != 0)
        {
            CAgent* obj = src[i].m_ptr;
            slot->m_ptr = obj;
            if (obj)
                obj->m_refcount++;
        }
    }

    m_count = count;
}
