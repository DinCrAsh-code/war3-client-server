//============================================================================
//  NIpse::CPoPosCl - vtable slots 15 (ComputeChecksum) and 16
//  (DumpDebugState), shared with NTempest::CLinkedList<NIpse::CPoPosCl>.
//  See poposcl.h for the class-level header comment.
//============================================================================
#include "poposcl.h"
#include "textsink.h"

namespace NIpse {

static const char ValueName[] = "";

static inline unsigned int PoPosClByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

static inline void PoPosClFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += PoPosClByteFold((hi >> 8) & 0xFFu);
    *hash += PoPosClByteFold(hi & 0xFFu)
           + PoPosClByteFold((v >> 8) & 0xFFu)
           + PoPosClByteFold(v & 0xFFu);
}

typedef void (__thiscall *ComputeChecksumFn)(void*, unsigned int*);

//----------------------------------------------------------------------------
//  0x6F4946D0 - vtable slot 15, ComputeChecksum. Folds the base
//  `CPoPos::ComputeChecksum` first, then thirteen of this class's own
//  fields (offset order: +0x9C, +0xA0, +0xB0, +0xBC, +0xB4, +0xB8, +0xC0,
//  +0xC4, +0xC8, +0xCC, +0xD0, +0xD4, +0xD8 - NOT strictly ascending,
//  matching the dump exactly), and finally - if the embedded
//  `m_poSeparate` is live - its own `ComputeChecksum` through its own
//  vtable slot 9 (Agent/poseparate.h), a real indirect dispatch in the
//  shipped code (not a direct call).
//----------------------------------------------------------------------------
void CPoPosCl::ComputeChecksum(unsigned int* hash)
{
    ((CPoPos*)this)->ComputeChecksum(hash);

    const char* base = (const char*)this;
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0x9C));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xA0));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xB0));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xBC));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xB4));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xB8));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xC0));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xC4));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xC8));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xCC));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xD0));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xD4));
    PoPosClFoldDword(hash, *(const unsigned int*)(base + 0xD8));

    if (m_poSeparate)
    {
        ComputeChecksumFn fn = (ComputeChecksumFn)(*(void***)m_poSeparate)[9];
        fn(m_poSeparate, hash);
    }
}

//----------------------------------------------------------------------------
//  0x6F494AD0 - vtable slot 16, DumpDebugState. Resolved this follow-up
//  session: `worktree_store.py show 0x6F494AD0` carries the FULL raw_asm
//  the earlier session's own dump fragment cut short, and
//  `worktree_store.py resolve aLmgSLdlSLorSLo` gives the untruncated format
//  string - "\tlmg[%s] ldl[%s] lor[%s] lod[%s] pcl x[%u] y[%u] opt[0x%08X]" -
//  which settles both the field count (seven, not the five this session's
//  earlier guess would have needed) and, working the argument order back
//  from the shipped push sequence, a REAL correction to this file's own
//  field layout for +0xC0..+0xD8 (poposcl.h's own header comment already
//  updated - see it for the byte-level story): the four FormatCFloat calls
//  print +0xC0/+0xC4/+0xC8/+0xCC as `lmg`/`ldl`/`lor`/`lod` (one slot
//  earlier than this file's own guessed names had them), then three plain
//  dwords print +0xD0/+0xD4/+0xD8 as `x`/`y`/`opt` - `pclY` (+0xD4) and
//  `m_opt` (+0xD8, an options bitfield, not the anonymous zeroed field this
//  session used to guess) are new names this trace adds. The earlier
//  "\tglr[%s] prw[%s] acc[%s] ang[%s]" line has the same kind of
//  correction: working ITS OWN push order back the same way gives
//  glr=+0xB0, acc=+0xB4, ang=+0xB8, prw=+0xBC - not the
//  glr/prw/[relatedCount]/acc guess this file's header comment used to
//  carry (there is no related-agent COUNT field on CPoPosCl at all - see
//  poposcl.h's own updated header comment on why the +0xA8 field this
//  class actually has is a single related-agent pointer, not an array).
//----------------------------------------------------------------------------
void CPoPosCl::DumpDebugState(void* sink)
{
    ((CPoPos*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[PosCl]");

    const char* base = (const char*)this;
    LogFormatted(sink, "\tclus t[%u] b[%u]",
                 *(const unsigned int*)(base + 0x9C),
                 *(const unsigned int*)(base + 0xA0));

    LogFormatted(sink, "\tglr[%s] prw[%s] acc[%s] ang[%s]",
                 FormatCFloat((const CFloat*)(base + 0xB0), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xBC), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xB4), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xB8), 1, -1));

    LogFormatted(sink, "\tlmg[%s] ldl[%s] lor[%s] lod[%s] pcl x[%u] y[%u] opt[0x%08X]",
                 FormatCFloat((const CFloat*)(base + 0xC0), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xC4), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xC8), 1, -1),
                 FormatCFloat((const CFloat*)(base + 0xCC), 1, -1),
                 *(const unsigned int*)(base + 0xD0),
                 *(const unsigned int*)(base + 0xD4),
                 *(const unsigned int*)(base + 0xD8));

    if (m_poSeparate != 0)
    {
        typedef void (__thiscall *DumpDebugStateFn)(void*, void*);
        DumpDebugStateFn fn = (DumpDebugStateFn)(*(void***)m_poSeparate)[10];
        fn(m_poSeparate, sink);
    }
}

}  // namespace NIpse
