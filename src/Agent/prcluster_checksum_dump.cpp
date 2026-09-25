//============================================================================
//  NIpse::CPrCluster - vtable slots 15/16 (ComputeChecksum/DumpDebugState),
//  the cluster-D follow-up-3 session (2026-09-12). Own translation unit:
//  neither calls anything else declared in prcluster_saveload.cpp/
//  prcluster_dtor.cpp, and the two share the same +0x40..+0x80 field span
//  and the same record-table walk, so keeping them together mirrors
//  CPrClusterGroup's own prclustergroup_slots2.cpp grouping.
//============================================================================
#include "prcluster.h"
#include "presence.h"
#include "textsink.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  Identical byte-fold-and-add idiom Agent/prclustergroup_slots2.cpp's own
//  PrClusterGroupByteFold/PrClusterGroupFoldDword already name for their own,
//  unrelated caller - duplicated locally per that file's own precedent.
//----------------------------------------------------------------------------
static inline unsigned int PrClusterByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PrClusterFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    unsigned int folded = PrClusterByteFold((hi >> 8) & 0xFFu);
    folded += PrClusterByteFold(hi & 0xFFu);
    folded += PrClusterByteFold((v >> 8) & 0xFFu);
    folded += PrClusterByteFold(v & 0xFFu);
    *hash += folded;
}

//  0x6F486C10 - vtable slot 15.  See prcluster.h's own doc comment for the
//  exact field list (only 5 of each record's own 9 fields are folded, and
//  four of the +0x40..+0x80 span's own fourteen are skipped - both counts
//  confirmed off the dump).
void CPrCluster::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    SPrClusterRecord* records = (SPrClusterRecord*)m_records.m_block.m_data;
    for (unsigned int i = 0; i < m_records.m_count; i++)
    {
        PrClusterFoldDword(hash, records[i].m_field00);
        PrClusterFoldDword(hash, records[i].m_field04);
        PrClusterFoldDword(hash, records[i].m_pair0C[0].m_bits);
        PrClusterFoldDword(hash, records[i].m_pair0C[1].m_bits);
        PrClusterFoldDword(hash, records[i].m_field28);
    }

    PrClusterFoldDword(hash, m_field40);
    PrClusterFoldDword(hash, m_field44);
    PrClusterFoldDword(hash, m_field4C.m_bits);
    PrClusterFoldDword(hash, m_field50.m_bits);
    PrClusterFoldDword(hash, m_field54.m_bits);
    PrClusterFoldDword(hash, m_field58.m_bits);
    PrClusterFoldDword(hash, m_field5C);
    PrClusterFoldDword(hash, m_field60);
    PrClusterFoldDword(hash, m_field64);
    PrClusterFoldDword(hash, m_field68);
    PrClusterFoldDword(hash, m_field6C);
    PrClusterFoldDword(hash, m_field70.m_bits);
    PrClusterFoldDword(hash, m_field74.m_bits);
    PrClusterFoldDword(hash, m_field80);
}

//  0x6F4871C0 - vtable slot 16.  Every field/format-string pairing here is
//  read straight off the dump's own push order (LogFormatted is __cdecl,
//  so its own varargs push in reverse of the call's own argument order) -
//  see this class's own header comment for the once-per-record line and
//  the four trailing summary lines.
void CPrCluster::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_tagT, m_tagB);
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrCl]");
    LogFormatted(sink, "\t[members]");

    SPrClusterRecord* records = (SPrClusterRecord*)m_records.m_block.m_data;
    for (unsigned int i = 0; i < m_records.m_count; i++)
    {
        LogFormatted(sink, "\t\t%u -> t[%u] b[%u] ofs x[%s] y[%s] opt[0x%X]",
                     i, records[i].m_field00, records[i].m_field04,
                     FormatCFloat(&records[i].m_pair0C[0], 1, -1),
                     FormatCFloat(&records[i].m_pair0C[1], 1, -1),
                     records[i].m_field28);
    }

    LogFormatted(sink, "");
    LogFormatted(sink, "\ttgt t[%u] b[%u]", m_field40, m_field44);
    LogFormatted(sink, "\tofs x[%s] y[%s]",
                 FormatCFloat(&m_field4C, 1, -1),
                 FormatCFloat(&m_field50, 1, -1));
    LogFormatted(sink, "\tway x[%s] y[%s]",
                 FormatCFloat(&m_field54, 1, -1),
                 FormatCFloat(&m_field58, 1, -1));
    LogFormatted(sink, "\tage[%u] wai[%u] ghy[%u] why[%u] fog[%u] ori[%s] rot[%s]",
                 m_field5C, m_field60, m_field64, m_field68, m_field6C,
                 FormatCFloat(&m_field70, 1, -1),
                 FormatCFloat(&m_field74, 1, -1));
    LogFormatted(sink, "\topt[0x%X]", m_field80);
}

}  // namespace NIpse
