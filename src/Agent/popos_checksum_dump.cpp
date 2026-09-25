//============================================================================
//  NIpse::CPoPos - vtable slots 15 (ComputeChecksum) and 16 (DumpDebugState).
//  See popos.h for the class-level header comment.
//============================================================================
#include "popos.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

//  IDA's own (unresolved) name for the shared empty-string global,
//  0x6F87529C - same file-local copy convention Agent/rlproperty_dump.cpp
//  already uses.
static const char ValueName[] = "";

static inline unsigned int PoPosByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//  Same "top byte stored alone, other three summed together" shape as
//  Agent/rlproperty_checksum.cpp's own RlPropertyFoldDword - own local
//  copy, same "no shared translation unit" reasoning.
static inline void PoPosFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += PoPosByteFold((hi >> 8) & 0xFFu);
    *hash += PoPosByteFold(hi & 0xFFu)
           + PoPosByteFold((v >> 8) & 0xFFu)
           + PoPosByteFold(v & 0xFFu);
}

//----------------------------------------------------------------------------
//  0x6F487690 - vtable slot 15, ComputeChecksum. Folds the base
//  `CRlProperty::ComputeChecksum` first, then this class's own seven added
//  CFloats (+0x78 through +0x90 inclusive, in offset order).
//----------------------------------------------------------------------------
void CPoPos::ComputeChecksum(unsigned int* hash)
{
    ((CRlProperty*)this)->ComputeChecksum(hash);

    const char* base = (const char*)this;
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x78));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x7C));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x80));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x84));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x88));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x8C));
    PoPosFoldDword(hash, *(const unsigned int*)(base + 0x90));
}

//----------------------------------------------------------------------------
//  0x6F4878B0 - vtable slot 16, DumpDebugState. Dumps
//  `CAgentRelation::DumpDebugState` DIRECTLY (0x6F4A62D0) and then
//  duplicates `CRlProperty::DumpDebugState`'s own "[Prop]"-tagged
//  +0x70/+0x74 line by hand (same output either way, but the shipped
//  code calls the CAgentRelation leaf directly rather than through
//  CRlProperty's own wrapper - Agent/rlproperty_dump.cpp's own
//  0x6F4874F0 is NOT called from here), before this object's own
//  "[Pos]"-tagged "base x/y vel x/y" line over +0x78/+0x7C/+0x80/+0x84.
//----------------------------------------------------------------------------
void CPoPos::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Prop]");
    LogFormatted(sink, "\tbase[%s]:[%u]",
                 FormatCFloat((const CFloat*)((char*)this + 0x70), 1, -1),
                 *(const unsigned int*)((char*)this + 0x74));

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[Pos]");
    LogFormatted(sink, "\tbase x[%s] y[%s] vel x[%s] y[%s]",
                 FormatCFloat(&m_baseX, 1, -1), FormatCFloat(&m_baseY, 1, -1),
                 FormatCFloat(&m_velX, 1, -1), FormatCFloat(&m_velY, 1, -1));
    LogFormatted(sink, "\tmaxv[%s] ori[%s] rad[%s]",
                 FormatCFloat(&m_fieldD, 1, -1), FormatCFloat(&m_fieldE, 1, -1),
                 FormatCFloat(&m_fieldF, 1, -1));
}

}  // namespace NIpse
