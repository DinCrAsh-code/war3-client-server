//============================================================================
//  0x6F255800 - `retn 4`.  CUnit vtable slot 72's own depth-1 closure: scale
//  two CFloats eight bytes apart (a padding dword sits between them) by the
//  same factor, in place.  Which object and what the two fields mean is not
//  established by this call tree - only the two offsets it actually
//  touches get names, per CLAUDE.md's own rule.
//============================================================================
#include "CFloat.h"

struct SScaledDimensionPair
{
    char   m_reserved00[0x10];
    CFloat m_dim10;              // +0x10
    char   m_reserved14[0x18 - 0x14];
    CFloat m_dim18;              // +0x18

    void ScaleBoth(const CFloat* factor);
};

void SScaledDimensionPair::ScaleBoth(const CFloat* factor)
{
    m_dim10 = m_dim10 * *factor;
    m_dim18 = m_dim18 * *factor;
}
