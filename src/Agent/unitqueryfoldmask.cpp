//============================================================================
//  0x6F25D830 - FoldQueryClassMask: turn variadic tag 29's three words into
//  the bits they contribute to a unit enumeration's own mask.
//
//  The three words are a relationship filter, the unit the relationships
//  are measured against, and an explicit player id for when there is no
//  unit (agenteventbinding.cpp fills all three from one tag).  The filter
//  goes through ApplyUnitFilterDefaults first, so a caller that named none
//  of a group gets that group's default; everything after works on the
//  filled-in mask, never on the raw one.
//
//  What comes out is 0x00FF0000 - "no player restriction" - unless any of
//  the four owner-relationship bits 0x1E is set, in which case the low word
//  becomes the excluded-player set CUnit::RelationPlayerMask computes
//  (unitrelationplayermask.cpp) and the 0x00FD0000 pattern replaces
//  0x00FF0000.  Three further bits then clear or set one flag each.  The
//  `invert` argument to RelationPlayerMask is a literal 1 because the
//  enumeration mask's low word is a *reject* set, the same convention
//  BuildAndRegisterEventBinding's own `mask |= ~m_playerMask & 0xFFFF`
//  already follows.
//
//  Both remaining tests are "bit clear means set the flag", not the other
//  way round: 0x01000000 absent adds 0x02000000 and 0x02000000 absent adds
//  0x01000000, which is the shipped `jnz` over each `or`.
//
//  Own translation unit: it calls ApplyUnitFilterDefaults and
//  CUnit::RelationPlayerMask for real, and its sibling 0x6F25D480 is in
//  the same shipped module.
//============================================================================
#include "agenteventbinding.h"
#include "unit.h"

unsigned int __fastcall FoldQueryClassMask(int relations, int source,
                                           int player)
{
    unsigned int bits = ApplyUnitFilterDefaults((unsigned int)relations);

    unsigned int mask = 0x00FF0000;
    if ((bits & 0x1E) != 0)
        mask = (((CUnit*)source)->RelationPlayerMask(bits, player, 1) & 0xFFFF)
               | 0x00FD0000;

    if ((bits & 0x7C0) != 0)
        mask &= ~0x00040000u;
    if ((bits & 0x20) != 0)
        mask &= ~0x00080000u;
    if ((bits & 0x01000000) == 0)
        mask |= 0x02000000;
    if ((bits & 0x02000000) == 0)
        mask |= 0x01000000;

    return mask;
}
