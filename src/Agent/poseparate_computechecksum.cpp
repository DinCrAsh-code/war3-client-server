//============================================================================
//  0x6F4843C0 - NIpse::CPoSeparate::ComputeChecksum, vtable slot 9. Folds
//  m_sepvX, m_sepvY and m_opt (in that offset order, +0x18/+0x1C/+0x20)
//  into *hash - the same per-byte `(b*0x7A2D)^b` fold, four bytes summed
//  by addition (most-significant byte first), that
//  Agent/agentbaseabscomputechecksum.cpp already establishes for
//  NTempest::CPresenceTagged::FoldLinkFieldsIntoHash and
//  CAgentRelation::ComputeChecksum elsewhere in this family. Own copy of
//  the helper (own translation unit, no shared header for it yet) - same
//  reasoning that file's own local `AgentBaseAbsByteFold`/
//  `AgentBaseAbsFoldDword` give for not sharing one.
//
//  `this` in ecx, the `unsigned int*` accumulator in a single stack
//  argument (`retn 4`), matching every other ComputeChecksum-shaped slot
//  in this family.
//============================================================================
#include "poseparate.h"

namespace NIpse {

static inline unsigned int PoSeparateByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PoSeparateFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    unsigned int folded = PoSeparateByteFold((hi >> 8) & 0xFFu);
    folded += PoSeparateByteFold(hi & 0xFFu);
    folded += PoSeparateByteFold((v >> 8) & 0xFFu);
    folded += PoSeparateByteFold(v & 0xFFu);
    *hash += folded;
}

void CPoSeparate::ComputeChecksum(unsigned int* hash)
{
    PoSeparateFoldDword(hash, (unsigned int)m_sepvX.m_bits);
    PoSeparateFoldDword(hash, (unsigned int)m_sepvY.m_bits);
    PoSeparateFoldDword(hash, m_opt);
}

}  // namespace NIpse
