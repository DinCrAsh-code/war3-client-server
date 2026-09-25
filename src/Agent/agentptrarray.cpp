//============================================================================
//  TSGrowableArray<CAgentPtr> - a growable array of reference-counted agent
//  slots.  The shipped code tags its Storm calls with the element type's own
//  RTTI descriptor name (".?AV?$TSGrowableArray@VCAgentPtr@@..."), which is
//  what identifies the instantiation.
//
//  0x6F286590 ~TSGrowableArray.
//
//  Unlike the pointer and int instantiations (ptrarray.cpp, intarray.cpp),
//  this one's element destructor is *not* trivial: CAgentPtr::~CAgentPtr
//  releases its reference, so the generic template's "destroy every live
//  element, then free the block" loop survives inlining with a real body -
//  the test-decrement-call-slot-0 three instructions agent.h documents.
//  That is why this destructor has a loop where ptrarray.cpp's has none.
//
//  Written out here as an explicit specialisation rather than as a plain
//  `template TSGrowableArray<CAgentPtr>::~TSGrowableArray();` for one
//  measured reason.  The shipped loop loads the agent pointer once
//  (`mov ecx, [eax+esi*4]`) and does the whole test-decrement-call out of
//  that register; CAgentPtr::Release(), which the generic body reaches
//  through ~CAgentPtr, reads m_ptr afresh after the decrement and costs a
//  reload.  Respelling ~CAgentPtr itself with a local fixes this function
//  (+4) but takes an instruction off three already-matched targets that
//  inline it (-3), which is a regression the report flags - so the local
//  lives here, in the one body that wants it, and CAgentPtr keeps the
//  spelling every other call site already agrees with.
//
//  The dead `lea eax, [eax + esi*4]` in the shipped stream is the address of
//  the element being destroyed: `m_data[i].~T()` computes it whether or not
//  the inlined body needs the pointer itself.
//============================================================================
#include "tsarray.inl"
#include "agent.h"

template <>
TSGrowableArray<CAgentPtr>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
    {
        CAgent* obj = m_data[i].m_ptr;
        if (obj)
        {
            if (--obj->m_refcount == 0)
                obj->ReleaseSelf();
        }
    }

    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}
