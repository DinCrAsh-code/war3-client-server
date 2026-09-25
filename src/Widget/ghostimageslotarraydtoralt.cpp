//============================================================================
//  0x6F2ADBA0 - a second TSGrowableArray<T>::~TSGrowableArray() over a
//  plain, type-*unchecked* CAgentPtr<CGhostImage> (the raw
//  `push offset aAvCagentptrVcg` tag, ".?AV?$CAgentPtr@VCGhostImage@@@@"),
//  at a different address from the already-reconstructed
//  TSGrowableArray<SCheckedGhostImageSlot> family (0x6F2ABB10/0x6F2ACCA0,
//  Containers/agentptrarray_thunks.cpp / ghostimageagentptrcount.cpp,
//  Containers/ghostimageslot.h) - same debug tag, same element shape,
//  genuinely different instantiation (the image was linked without
//  /OPT:ICF - see Agent/agenttypedslots.h's own note on why that leaves
//  identical bodies at distinct addresses).  Kept in its own local element
//  type here rather than reusing SCheckedGhostImageSlot, so this TU's
//  instantiation is a distinct symbol from that family's.
//
//  0x6F2AE8C0 is the same "second address, no /OPT:ICF" duplicate this
//  batch's own 0x6F28D4D0 is for the CWidget array's destructor - a naked
//  redirect into the one body above.
//============================================================================
#include "agent.h"

struct SGhostImagePtrSlotAlt
{
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

#define TSARRAY_TYPENAME ".?AV?$CAgentPtr@VCGhostImage@@@@"
#include "tsarray.inl"

//  0x6F2ADBA0
template <>
TSGrowableArray<SGhostImagePtrSlotAlt>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
        m_data[i].Release();
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//  0x6F2AE8C0
__declspec(naked) void DestructGhostImageSlotArrayAliasAlt_6F2AE8C0()
{
    __asm { mov eax, 06F2ADBA0h }
    __asm { jmp eax }
}
