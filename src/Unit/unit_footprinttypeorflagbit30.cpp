//============================================================================
//  0x6F03FDC0 - CUnit::TestFootprintTypeRangeAndFlagBit30Clear (unit.h).
//
//  True only if m_footprintType's own first (most-significant) byte -
//  stored MSB-first, the same convention agiletype.h's own
//  kAgileRootTypeId is written in - is an uppercase ASCII letter, AND
//  flag bit 30 of m_flags5C is clear.  Both conditions share the same
//  false exit (docs/msvc-vc8-idioms.md's "Shared exits need goto") rather
//  than being independent early returns, which is what keeps the second
//  test branchy instead of collapsing into a branchless mask.
//============================================================================
#include "unit.h"

int CUnit::TestFootprintTypeRangeAndFlagBit30Clear()
{
    unsigned char firstChar = *((unsigned char*)&m_footprintType + 3);
    if ((unsigned int)firstChar + 0xFFFFFFBFu > 0x19)
        goto notOk;
    if (m_flags5C & 0x40000000)
        goto notOk;
    return 1;
notOk:
    return 0;
}
