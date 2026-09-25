//============================================================================
//  0x6F487440 - NIpse::CRlProperty::ComputeChecksum, vtable slot 15 (+0x3C).
//  Folds the base `CAgentRelation::ComputeChecksum` first (0x6F4A60A0,
//  agentrelation.h), then this class's own two added dwords at +0x70/+0x74,
//  using the identical per-byte `(b*0x7A2D)^b` fold idiom
//  Agent/agentbaseabscomputechecksum.cpp already names - kept as its own
//  local copy here, same "no shared translation unit" reasoning that file
//  gives.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"

namespace NIpse {

static inline unsigned int RlPropertyByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//  Unlike Agent/agentbaseabscomputechecksum.cpp's own AgentBaseAbsFoldDword
//  (which sums all four byte-folds and stores once), this dword's own fold
//  stores to *hash TWICE: the top byte alone first, then the other three
//  summed together - two separate statements in the shipped source, not
//  one four-way sum (confirmed by the dump's own `add [esi],ebx` sitting
//  between the top-byte fold and the other three, with `[esi]` (`*hash`)
//  reloaded before the second sum starts).
static inline void RlPropertyFoldDword(unsigned int* hash, unsigned int v)
{
    //  Matches the dump's own extraction exactly: `shr eax,0x10` then
    //  `movzx ah`/`movzx al` off the shifted value for the top two bytes
    //  (Sync/ctimesynchashinto.cpp's own shape) - the top byte's fold is
    //  stored to *hash on its own first, then the other three summed and
    //  stored together, matching the dump's own `add [esi],ebx` sitting
    //  between them with `[esi]` (`*hash`) reloaded before the second sum.
    unsigned int hi = v >> 16;
    *hash += RlPropertyByteFold((hi >> 8) & 0xFFu);
    *hash += RlPropertyByteFold(hi & 0xFFu)
           + RlPropertyByteFold((v >> 8) & 0xFFu)
           + RlPropertyByteFold(v & 0xFFu);
}

void CRlProperty::ComputeChecksum(unsigned int* hash)
{
    ((CAgentRelation*)this)->ComputeChecksum(hash);

    const char* base = (const char*)this;
    RlPropertyFoldDword(hash, *(const unsigned int*)(base + 0x70));
    RlPropertyFoldDword(hash, *(const unsigned int*)(base + 0x74));
}

}  // namespace NIpse
