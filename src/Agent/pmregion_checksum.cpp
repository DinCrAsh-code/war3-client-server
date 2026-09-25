//============================================================================
//  NIpse::CPmRegion::ComputeChecksum, 0x6F49E640, vtable slot 15 (+0x3C).
//  See pmregion.h for the class-level header comment.
//
//  This class's OWN override (NOT the shared
//  NTempest::CPresenceTagged::FoldLinkFieldsIntoHash the other four classes
//  in this cluster inherit unchanged - Agent/basemap.h's own file header).
//  Calls that real base fold first (0x6F4AEE60, Agent/presence.h,
//  Agent/agentbaseabscomputechecksum.cpp), then folds this class's own five
//  added fields (+0x1C/+0x20/+0x24/+0x28/+0x34, in that order) using the
//  identical `(b*0x7A2Du)^b` per-byte scramble / add-not-xor combine
//  Agent/agentbaseabscomputechecksum.cpp already names for the same family
//  of callers - a fresh local copy here, not a shared header function, for
//  the same "no shared translation unit" reason that file gives for its
//  own copy.
//============================================================================
#include "pmregion.h"
#include "presence.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  A single byte, widened to 32 bits, run through the `(b*0x7A2D)^b`
//  scramble.
//----------------------------------------------------------------------------
static inline unsigned int PmRegionByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//----------------------------------------------------------------------------
//  Fold one dword's four bytes (most-significant first) into *hash via FOUR
//  SEPARATE `*hash += ...` statements, not one combined expression - matches
//  the dump's own shape exactly, including the one real asymmetry: the
//  first fold is a genuine read-modify-write straight to memory
//  (`add [esi],ebx`), while the second/third/fourth stay in registers and
//  write `*hash` back only once at the end (`mov edx,[esi]` once, after the
//  first add, then pure register arithmetic through to a single final
//  `mov [esi],edx`) - the compiler fusing four independent statements
//  rather than four independent memory round-trips. A single
//  `folded = a+b+c+d; *hash += folded;` shape (this file's own first,
//  wrong, attempt) reorders which byte's fold gets computed before which
//  add and does not reproduce this. Byte extraction: `shr eax,0x10` then
//  `movzx ah`/`movzx al` off the shifted value for the top two bytes,
//  `movzx ch`/`movzx cl` off the unshifted value for the bottom two.
//----------------------------------------------------------------------------
static inline void PmRegionFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += PmRegionByteFold((hi >> 8) & 0xFFu);
    *hash += PmRegionByteFold(hi & 0xFFu);
    *hash += PmRegionByteFold((v >> 8) & 0xFFu);
    *hash += PmRegionByteFold(v & 0xFFu);
}

//  0x6F49E640 - vtable slot 15 (+0x3C).
void CPmRegion::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    PmRegionFoldDword(hash, m_top);
    PmRegionFoldDword(hash, m_left);
    PmRegionFoldDword(hash, m_bottom);
    PmRegionFoldDword(hash, m_right);
    PmRegionFoldDword(hash, m_mask);
}

}  // namespace NIpse
