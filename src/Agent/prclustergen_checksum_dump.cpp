//============================================================================
//  NIpse::CPrClusterGen - vtable slots 15/16 (ComputeChecksum/DumpDebugState),
//  0x6F491790/0x6F491BC0.  Own translation unit: neither calls anything
//  else declared in prclustergen_ctordtor.cpp/prclustergen_helpers.cpp/
//  prclustergen_saveload.cpp, and the same "checksum/dump grouped together,
//  own TU" shape prcluster_checksum_dump.cpp/prclustergroup_slots2.cpp
//  already use for their own siblings.
//============================================================================
#include "prcluster.h"
#include "presence.h"
#include "textsink.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  Identical byte-fold-and-add idiom prcluster_checksum_dump.cpp's own
//  PrClusterByteFold/PrClusterFoldDword already name for their own,
//  unrelated caller - duplicated locally per that file's own precedent.
//----------------------------------------------------------------------------
static inline unsigned int PrClusterGenByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PrClusterGenFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    unsigned int folded = PrClusterGenByteFold((hi >> 8) & 0xFFu);
    folded += PrClusterGenByteFold(hi & 0xFFu);
    folded += PrClusterGenByteFold((v >> 8) & 0xFFu);
    folded += PrClusterGenByteFold(v & 0xFFu);
    *hash += folded;
}

//  0x6F491790 - vtable slot 15.  This class's own base contribution
//  (NTempest::CPresenceTagged::FoldLinkFieldsIntoHash) first, then every
//  one of the fixed twelve `m_refSlots` - only the `m_handle`/`m_typeTag`
//  pair of each (m_field08 is genuinely skipped, confirmed off the dump -
//  the same "only some of each element's own fields are folded" shape
//  CPrCluster::ComputeChecksum's own record-table loop already documents) -
//  then six of the +0xDC..+0x100 span's own ten scalar/CFloat fields in
//  strict offset order (+0xE4/+0xF0/+0xF4/+0xF8 genuinely skipped - the
//  same four-gap shape Save/Load's own doc comment on prcluster.h already
//  names for this span, minus the two handle-tag-pair pointers those
//  bodies validate/stream and this one does not read at all).
void CPrClusterGen::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    for (unsigned int i = 0; i < 12; i++)
    {
        PrClusterGenFoldDword(hash, m_refSlots[i].m_handle);
        PrClusterGenFoldDword(hash, m_refSlots[i].m_typeTag);
    }

    PrClusterGenFoldDword(hash, m_field0DC);
    PrClusterGenFoldDword(hash, m_field0E0);
    PrClusterGenFoldDword(hash, m_field0E8.m_bits);
    PrClusterGenFoldDword(hash, m_field0EC.m_bits);
    PrClusterGenFoldDword(hash, m_field0FC.m_bits);
    PrClusterGenFoldDword(hash, m_field100);
}

//  0x6F491BC0 - vtable slot 16.  Same "[PrTg] t[%u] b[%u]" header line as
//  every sibling in this family, then "[PrClGen]"/"\t[candidates]" and one
//  "\t\t%u -> t[%u] b[%u]" line per `m_refSlots` element (own index, own
//  `m_handle`/`m_typeTag` pair - the same fields ComputeChecksum above
//  folds), then the +0xDC..+0x100 span in four LogFormatted calls matching
//  the shipped body's own line breaks - x/y swapped the same way
//  CPrCluster::DumpDebugState's own doc comment already flags for its
//  sibling pair.
void CPrClusterGen::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_tagT, m_tagB);
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrClGen]");
    LogFormatted(sink, "\t[candidates]");

    for (unsigned int i = 0; i < 12; i++)
    {
        LogFormatted(sink, "\t\t%u -> t[%u] b[%u]",
                     i, m_refSlots[i].m_handle, m_refSlots[i].m_typeTag);
    }

    LogFormatted(sink, "");
    LogFormatted(sink, "\ttgt t[%u] b[%u]", m_field0DC, m_field0E0);
    LogFormatted(sink, "\tofs x[%s] y[%s]",
                 FormatCFloat(&m_field0E8, 1, -1),
                 FormatCFloat(&m_field0EC, 1, -1));
    LogFormatted(sink, "\trot[%s]", FormatCFloat(&m_field0FC, 1, -1));
    LogFormatted(sink, "\topt[0x%X]", m_field100);
}

}  // namespace NIpse
