//============================================================================
//  0x6F379640 - TSGrowableArray<CAgentPtr>::ReleaseAndReserve.
//
//  Destroy every live element, then resize the block to hold `alloc` of
//  them.  Not SetAlloc: the release loop runs from zero rather than from
//  `alloc`, Storm is asked with flags 0 rather than the "resize in place or
//  fail" 0x10, and there is no alloc/copy/free fallback behind the resize
//  because there is nothing left to copy.
//
//  The reallocation is skipped entirely when there is neither an old block
//  nor a new size, which is what the shipped `test eax,eax` / `jnz` /
//  `test ecx,ecx` / `jz` pair is.
//
//  Written out rather than instantiated for the reason Agent/agentptrarray
//  .cpp gives for the destructor: the shipped loop loads the agent pointer
//  once and does the whole test-decrement-call out of that register, and
//  CAgentPtr::Release() re-reads m_ptr after the decrement.
//
//  Own translation unit: TSGrowableArray<CAgentPtr>::Assign calls it for
//  real.
//============================================================================
#include "tsarray.inl"
#include "agent.h"

template <>
void TSGrowableArray<CAgentPtr>::ReleaseAndReserve(unsigned int alloc)
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

    CAgentPtr* olddata = m_data;
    m_alloc = alloc;
    if (olddata != 0 || alloc != 0)
        m_data = (CAgentPtr*)SMemReAlloc(olddata, alloc * sizeof(CAgentPtr),
                                         s_typeName, -2, 0);
}
