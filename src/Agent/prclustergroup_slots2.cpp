//============================================================================
//  NIpse::CPrClusterGroup - vtable slots 3/4/9/10/15/16 (Apply, Slot4,
//  Save, Load, ComputeChecksum, DumpDebugState), the cluster-D follow-up
//  session (NTempestNIpse cluster D, 2026-09-12).  See prcluster.h's own
//  per-method comments for the full per-slot writeup; this file carries
//  only the bodies.
//
//  Grouped in one file: none of these six calls another body in this same
//  file (Apply/Load call the shared SAgentLiveInstanceListHead::InsertAtHead,
//  but only its DECLARATION is visible here - agentliveinstancelist.cpp's
//  own header explains why its definition must stay out of line), so
//  nothing here can inline into anything else here.
//============================================================================
#include "prcluster.h"
#include "presence.h"
#include "agentliveinstancelist.h"
#include "handletable.h"
#include "game.h"
#include "floatmini.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"
#include "textsink.h"

//  0x6F6EFE00/0x6F6EED50 - already reconstructed (Net/cdatastore_writecfloat.cpp,
//  Net/cdatastore_readcfloat.cpp), declared locally the same way every
//  other caller in this codebase does (no shared header exists for either).
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

//----------------------------------------------------------------------------
//  Identical byte-fold-and-add idiom Agent/agentbaseabscomputechecksum.cpp's
//  own AgentBaseAbsByteFold/AgentBaseAbsFoldDword and
//  Missile/bhpoprojectile_checksum_dump.cpp's own BhPoByteFold/BhPoFoldDword
//  already name for their own, unrelated callers - duplicated locally per
//  those files' own precedent (no shared translation unit across Agent/
//  and Missile/).
//----------------------------------------------------------------------------
static inline unsigned int PrClusterGroupByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PrClusterGroupFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    unsigned int folded = PrClusterGroupByteFold((hi >> 8) & 0xFFu);
    folded += PrClusterGroupByteFold(hi & 0xFFu);
    folded += PrClusterGroupByteFold((v >> 8) & 0xFFu);
    folded += PrClusterGroupByteFold(v & 0xFFu);
    *hash += folded;
}

//  0x6F48ED30 - vtable slot 3, Apply.  See prcluster.h's own doc comment.
void CPrClusterGroup::Apply(SHandleRegistrationRequest* arg)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(arg);

    SAgentLiveInstanceListHead* liveList =
        (SAgentLiveInstanceListHead*)((char*)g_pTimeSync + 0x384);
    liveList->InsertAtHead(this);
}

//  0x6F48ED60 - vtable slot 4, Slot4.  See prcluster.h's own doc comment.
void CPrClusterGroup::Slot4(int arg)
{
    void* prev = m_prevLink;
    void* next = m_nextLink;
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    m_prevLink = 0;
    m_nextLink = 0;

    CFloat def = *(const CFloat*)&g_unk6FAAE624;
    m_vcr = def;
    m_vpr = def;
    m_lir = g_CFloatZero;

    ((SHandleTableEntry*)this)->UnregisterAndNotify(arg);
}

//  0x6F48EDA0 - vtable slot 9, Save.  See prcluster.h's own doc comment.
void CPrClusterGroup::Save(CDataStoreScratch* store)
{
    CFloat* vpr = &m_vpr;
    CFloat* lir = &m_lir;
    CFloat* vcr = &m_vcr;
    CDataStoreScratch* s = store->WriteDword(m_mbr);
    s = WriteCFloat(s, vpr);
    s = WriteCFloat(s, vcr);
    s = WriteCFloat(s, lir);
}

//  0x6F48EDE0 - vtable slot 10, Load.  See prcluster.h's own doc comment.
void CPrClusterGroup::Load(CDataStore* store)
{
    CFloat* vpr = &m_vpr;
    CFloat* lir = &m_lir;
    CFloat* vcr = &m_vcr;
    CDataStore* s = store->ReadDword(&m_mbr);
    s = ReadCFloat(s, vpr);
    s = ReadCFloat(s, vcr);
    s = ReadCFloat(s, lir);

    SAgentLiveInstanceListHead* liveList =
        (SAgentLiveInstanceListHead*)((char*)g_pTimeSync + 0x384);
    liveList->InsertAtHead(this);
}

//  0x6F47AE80 - vtable slot 15, ComputeChecksum.  See prcluster.h's own
//  doc comment.
void CPrClusterGroup::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    PrClusterGroupFoldDword(hash, m_mbr);
    PrClusterGroupFoldDword(hash, m_vpr.m_bits);
    PrClusterGroupFoldDword(hash, m_vcr.m_bits);
    PrClusterGroupFoldDword(hash, m_lir.m_bits);
}

//  0x6F47AFC0 - vtable slot 16, DumpDebugState.  See prcluster.h's own doc
//  comment.
void CPrClusterGroup::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_tagT, m_tagB);
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrClGroup]");
    LogFormatted(sink, "\tmbr[%u] vpr[%s] vcr[%s] lir[%s]",
                 m_mbr,
                 FormatCFloat(&m_vpr, 1, -1),
                 FormatCFloat(&m_vcr, 1, -1),
                 FormatCFloat(&m_lir, 1, -1));
}

}  // namespace NIpse
