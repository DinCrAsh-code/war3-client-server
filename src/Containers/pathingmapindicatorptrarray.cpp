//============================================================================
//  TSGrowableArray<TRefCntPtr<CPathingMapIndicator>> - tagged
//  ".?AV?$TRefCntPtr@VCPathingMapIndicator@@@@" (aAvTrefcntptrVc_5), a
//  different smart-pointer family from the CAgentPtr<X> ("SCheckedXxxSlot")
//  ones elsewhere in Containers/ - same four-byte layout and the same
//  Release() shape (`--m_refcount == 0` then a devirtualised call through
//  slot 0), but not type-checked against any FourCC the way a CAgentPtr's
//  own Assign is.
//
//  Only the destructor (0x6F3931D0) is in this batch's own closure;
//  0x6F3953A0 is a second, byte-identical instantiation the linker folded
//  to a `jmp` here rather than a second copy - the same pointer-template
//  folding docs/msvc-vc8-idioms.md documents (any T whose Release() has
//  this exact shape compiles to the same destructor, T itself untouched by
//  the body).
//============================================================================
#include "agent.h"
#include "storm.h"
#include "tsarray.inl"

class CPathingMapIndicator;

struct TRefCntPtr_PathingMapIndicator
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

template <>
TSGrowableArray<TRefCntPtr_PathingMapIndicator>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
    {
        TRefCntPtr_PathingMapIndicator* element = &m_data[i];
        element->Release();
    }
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//  0x6F3953A0 - a second, byte-identical instantiation of the same
//  destructor the linker folded to a plain jmp here rather than a second
//  copy.  Reproduced the same way, as a naked redirect to the canonical
//  body above.
__declspec(naked) void SecondTRefCntPtrArrayDtorAlias()
{
    __asm
    {
        mov     eax, 06F3931D0h
        jmp     eax
    }
}
