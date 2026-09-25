//============================================================================
//  0x6F2552F0 - ApplyUnitFilterDefaults: the unit-filter mask's own
//  "nothing asked for in this group means all of it" rule, applied to eight
//  groups at once and then ORed back over what the caller did ask for.
//
//  Every group is the same three-instruction branchless shape - `and` the
//  group out, `neg`/`sbb` it to 0 or -1, `and` that against (default - 0)
//  and add the default back - which is MSVC's lowering of
//  `(mask & GROUP) ? 0 : DEFAULT`.  Two of the eight have a default that is
//  *narrower* than the group they test (0x7FE defaults to 0xE, 0x300000 to
//  0x100000, 0x3000000 to 0x1000000), so the rule is "if you named none of
//  these, here is the usual one", not "here is all of them".
//
//  A __fastcall free function and not a member: the mask arrives in ecx,
//  nothing is pushed, and `retn 0` - a thiscall member would have made the
//  mask a `this` it never dereferences.
//
//  Own translation unit: FoldQueryClassMask calls it for real, and it is
//  its own shipped module (0x6F255xxx against 0x6F25Dxxx).
//============================================================================
#include "agenteventbinding.h"

unsigned int __fastcall ApplyUnitFilterDefaults(unsigned int filter)
{
    unsigned int result = (filter & 0x0001F000) ? 0 : 0x0001F000;

    result |= (filter & 0xC0000000) ? 0 : 0xC0000000;
    result |= (filter & 0x0C000000) ? 0 : 0x0C000000;
    result |= (filter & 0x30000000) ? 0 : 0x30000000;
    result |= (filter & 0x000007FE) ? 0 : 0x0000000E;
    result |= (filter & 0x03000000) ? 0 : 0x01000000;
    result |= (filter & 0x00C00000) ? 0 : 0x00C00000;
    result |= (filter & 0x00300000) ? 0 : 0x00100000;

    return result | filter;
}
