//============================================================================
//  NIpse::CPoPosBh - vtable slots 15 (ComputeChecksum) and 16 (DumpDebugState).
//  See poposbh.h for the class-level header comment.
//============================================================================
#include "poposbh.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

static inline unsigned int PoPosBhByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PoPosBhFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += PoPosBhByteFold((hi >> 8) & 0xFFu);
    *hash += PoPosBhByteFold(hi & 0xFFu)
           + PoPosBhByteFold((v >> 8) & 0xFFu)
           + PoPosBhByteFold(v & 0xFFu);
}

//----------------------------------------------------------------------------
//  0x6F48CAF0 - vtable slot 15, ComputeChecksum. Folds the base
//  `CPoPos::ComputeChecksum` first, then this class's own nine added
//  CFloats (+0xC0 through +0xE0 inclusive, in offset order).
//----------------------------------------------------------------------------
void CPoPosBh::ComputeChecksum(unsigned int* hash)
{
    ((CPoPos*)this)->ComputeChecksum(hash);

    const char* base = (const char*)this;
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xC0));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xC4));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xC8));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xCC));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xD0));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xD4));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xD8));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xDC));
    PoPosBhFoldDword(hash, *(const unsigned int*)(base + 0xE0));
}

//----------------------------------------------------------------------------
//  0x6F48CDB0 - vtable slot 16, DumpDebugState. Dumps the base
//  `CPoPos::DumpDebugState` first, then this object's own "[PosBh]" tag
//  and its own three field lines.
//----------------------------------------------------------------------------
void CPoPosBh::DumpDebugState(void* sink)
{
    ((CPoPos*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[PosBh]");
    LogFormatted(sink, "\tpdl[%s] for[%s] rot[%s]",
                 FormatCFloat(&m_pdl, 1, -1), FormatCFloat(&m_forField, 1, -1),
                 FormatCFloat(&m_rot, 1, -1));
    LogFormatted(sink, "\tacc[%s] ang[%s] lvl[%s] ldl[%s]",
                 FormatCFloat(&m_acc, 1, -1), FormatCFloat(&m_ang, 1, -1),
                 FormatCFloat(&m_lvl, 1, -1), FormatCFloat(&m_ldl, 1, -1));
    LogFormatted(sink, "\tbrk[%s] prw[%s]",
                 FormatCFloat(&m_brk, 1, -1), FormatCFloat(&m_prw, 1, -1));
}

}  // namespace NIpse
