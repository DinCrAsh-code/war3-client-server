//============================================================================
//  0x6F49B620 - zero m_stamp[start, end) where start/end fold `kind` the
//  same way ResetSearch's own +0x74 clamp does (pathfinding_lrpath_slot4,
//  pathclient_slot4.cpp): `kind == -1` means "the whole two-element array",
//  any other value means "from index `kind` to the end".  The shipped body
//  computes both bounds with a branchless `sub/neg/sbb/and` idiom rather
//  than a compare-and-select, so this straight-line C++ is filed DIFFERS
//  (pending angr) rather than claimed EXACT by construction - see
//  pathmove.h's own comment on this method.
//============================================================================
#include "pathmove.h"

void CPathClient::ResetStampsFrom(int kind)
{
    unsigned int start = (kind == -1) ? 0u : (unsigned int)kind;
    unsigned int end   = (kind == -1) ? 2u : (unsigned int)kind + 1u;

    for (unsigned int i = start; i < end; i++)
        m_stamp[i] = 0;
}
