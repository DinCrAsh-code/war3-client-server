//============================================================================
//  The two remaining TSList instantiations the frame classes embed:
//  TSList<FRAMENODE, 3> (a CLayoutFrame's per-key node list, whose link sits
//  at +0x04 of each node) and TSList<LIGHTNODE, 0>.
//
//  Same reason framelist.cpp exists: UnlinkAll is declared out of line
//  because the shipped binary calls it rather than inlining it, so a class
//  that embeds one references a symbol nothing defines until some
//  translation unit names the member.  verify.py cannot see that - it never
//  links - and tools/link_check.py is what does.
//============================================================================
#include "frame.h"
#include "tsfixedlist.inl"

template void TSList<FRAMENODE, 4>::UnlinkAll();
template void TSList<LIGHTNODE, 0>::UnlinkAll();
