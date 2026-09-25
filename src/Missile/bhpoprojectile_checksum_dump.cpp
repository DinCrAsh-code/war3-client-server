//============================================================================
//  0x6F47A0A0 / 0x6F47A300 - CBhPoProjectile's own vtable slots 15/16
//  (ComputeChecksum, DumpDebugState), from the operator's 2026-09-07
//  RTTI-namespace vtable-dump fix (agent_worktrees/classes/0x6F951E04.json).
//  Part of the "cluster G" 4-class sweep alongside CBhPoBehavior/
//  CPoPoInterfLis/CPoPoVelocityMod.
//============================================================================
#include "bhpoprojectile.h"
#include "presence.h"
#include "textsink.h"

//----------------------------------------------------------------------------
//  Identical byte-fold-and-add idiom Agent/agentbaseabscomputechecksum.cpp's
//  own `AgentBaseAbsByteFold`/`AgentBaseAbsFoldDword` already name for their
//  own, unrelated callers - duplicated locally rather than shared, matching
//  that file's own precedent (Agent/ and Missile/ share no translation
//  unit).
//----------------------------------------------------------------------------
static inline unsigned int BhPoByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//  Combined form - one statement, four byte-folds summed before the one
//  store to *hash.  Used for the seven dwords after the first (see
//  BhPoFoldFirstDword just below for why the first one is different): the
//  dump's own dwords 2-8 each accumulate their own byte3's fold straight
//  into the register still holding the running hash (`add edx, ebp`) and
//  store to *hash exactly once per dword, never re-reading it - the same
//  shape this single-expression form produces.
static inline void BhPoFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    unsigned int folded = BhPoByteFold((hi >> 8) & 0xFFu);
    folded += BhPoByteFold(hi & 0xFFu);
    folded += BhPoByteFold((v >> 8) & 0xFFu);
    folded += BhPoByteFold(v & 0xFFu);
    *hash += folded;
}

//  The *first* dword is genuinely different in the dump: its own top
//  byte's fold goes through a real `add [esi],ebx` / `mov edx,[esi]`
//  memory round-trip (edx starts this function holding nothing useful,
//  so the top byte has to seed *hash for real before the other three
//  bytes can accumulate through a register) - a shape only a *separate*
//  top-level statement for that one byte reproduces; folding all four
//  into one expression (as BhPoFoldDword above does) lets the compiler
//  keep everything in registers and never re-read *hash at all, which is
//  the actual MISMATCH angr found (the two forms sum to the same value,
//  but angr's phase-B stub comparison caught the intermediate write angr
//  itself watches for on a symbolic pointer - `mem[...]` in its own
//  report).
static inline void BhPoFoldFirstDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += BhPoByteFold((hi >> 8) & 0xFFu);
    *hash += BhPoByteFold(hi & 0xFFu) + BhPoByteFold((v >> 8) & 0xFFu) + BhPoByteFold(v & 0xFFu);
}

//----------------------------------------------------------------------------
//  0x6F47A0A0 - vtable slot 15 (+0x3C), ComputeChecksum. Calls the base
//  `NTempest::CPresenceTagged::FoldLinkFieldsIntoHash` (0x6F4AEE60) first -
//  `this` reinterpreted as that class, the same
//  `((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash)` shape
//  `CAgentRelation::ComputeChecksum`
//  (Agent/agentbaseabscomputechecksum.cpp) already uses for the identical
//  base call - then folds this class's own 8 fields, in ascending offset
//  order, into the same accumulator.
//----------------------------------------------------------------------------
void CBhPoProjectile::ComputeChecksum(unsigned int* hash)
{
    CBhPoProjectile* self = this;

    ((NTempest::CPresenceTagged*)self)->FoldLinkFieldsIntoHash(hash);

    BhPoFoldFirstDword(hash, self->m_startX);
    BhPoFoldDword(hash, self->m_startY);
    BhPoFoldDword(hash, self->m_velX);
    BhPoFoldDword(hash, self->m_velY);
    BhPoFoldDword(hash, self->m_velMag);
    BhPoFoldDword(hash, self->m_velOri);
    BhPoFoldDword(hash, self->m_velTime);
    BhPoFoldDword(hash, self->m_velPeriod);
}

//----------------------------------------------------------------------------
//  0x6F47A300 - vtable slot 16, DumpDebugState. Four `LogFormatted` calls
//  (Storm/textsink.h, 0x6F4B73F0) then a run of `FormatCFloat`
//  (0x6F4B76E0) conversions feeding the last two - the same variadic sink
//  shape Agent/presencetagged_dumpdebugstate.cpp's own
//  `CPresenceTagged::DumpDebugState` already establishes for the "[PrTg]"
//  line (this class's own +0x14/+0x18 tag pair, printed with the identical
//  format string - matches CPresenceTagged's own `m_tagT`/`m_tagB`, since
//  ComputeChecksum above confirms `this` really does alias a
//  `CPresenceTagged` at that offset). Every `FormatCFloat` call passes
//  `base=1, precision=-1`; argument order confirmed by each call's own push
//  order (deepest push = last variadic argument, cdecl).
//----------------------------------------------------------------------------
void CBhPoProjectile::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_agentHandle, m_agentTypeTag);
    LogFormatted(sink, "");
    LogFormatted(sink, "[PosBhProj]");
    LogFormatted(sink, "\tstart x[%s] y[%s]",
                 FormatCFloat((const CFloat*)&m_startX, 1, -1),
                 FormatCFloat((const CFloat*)&m_startY, 1, -1));
    LogFormatted(sink, "\tvel x[%s] y[%s] mag[%s] ori[%s] tim[%s] prd[%u]",
                 FormatCFloat((const CFloat*)&m_velX, 1, -1),
                 FormatCFloat((const CFloat*)&m_velY, 1, -1),
                 FormatCFloat((const CFloat*)&m_velMag, 1, -1),
                 FormatCFloat((const CFloat*)&m_velOri, 1, -1),
                 FormatCFloat((const CFloat*)&m_velTime, 1, -1),
                 m_velPeriod);
}
