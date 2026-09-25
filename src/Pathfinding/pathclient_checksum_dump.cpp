//============================================================================
//  NIpse::CLrPath (CPathClient, pathmove.h) - vtable slots 15/16
//  (ComputeChecksum/DumpDebugState), 0x6F49CDE0/0x6F49CB70.  Own
//  translation unit: neither calls anything else declared in
//  pathclient_apply.cpp/pathclient_save.cpp/pathclient_load.cpp/etc, and
//  DumpDebugState's own real virtual dispatch into ComputeChecksum
//  (through slot 15, +0x3C) is the only cross-reference between the two.
//============================================================================
#include "pathmove.h"
#include "presence.h"
#include "textsink.h"

//----------------------------------------------------------------------------
//  Identical byte-fold-and-add idiom Agent/prcluster_checksum_dump.cpp's
//  own PrClusterByteFold/PrClusterFoldDword already name for their own,
//  unrelated caller - duplicated locally per that file's own precedent (no
//  shared translation unit across Agent/ and Pathfinding/).
//----------------------------------------------------------------------------
static inline unsigned int PathClientByteFold(unsigned int b)
{
    return (b * 0x7A2Du) ^ b;
}

//  Split as two compound assignments into `*hash` rather than one, and the
//  three remaining bytes folded in a DIFFERENT order than they are read
//  off `v` - both found this follow-up session by reading the shipped
//  stream instruction-by-instruction rather than guessed at from the
//  "logical" field order (docs/msvc-vc8-idioms.md has no existing entry
//  for either shape):
//
//  - the shipped stream folds the TOP byte (bits 24-31) straight into
//    `*hash` by itself, a standalone `add [hash], x` - not part of the
//    other three bytes' own running sum. Because `PathClientByteFold` is
//    pure and provably does not touch `*hash`, the compiler then keeps
//    the updated hash live in a register across every subsequent call in
//    the same straight-line sequence rather than re-reading memory each
//    time - which is what the dump shows for every field after the very
//    first (that one still pays a real memory load, since `*hash` there
//    crosses the `FoldLinkFieldsIntoHash` call boundary and cannot be
//    assumed cached);
//  - of the other three bytes, the shipped stream computes bits 8-15 and
//    bits 0-7 (both extracted straight off the original `ecx`, no shift
//    of the already-`shr`-by-16'd `eax` needed) before bits 16-23 (which
//    DOES need that shifted `eax`) - deferring the one extraction with a
//    data dependency on the first fold's own input register. Moving this
//    file's own `PathClientFoldDword` to the same evaluation order raised
//    the whole function from 0.147 (63/421) to 0.328 (141/421) - real
//    progress, not a full close: see this file's own `ComputeChecksum`
//    header comment for what is still open and why.
static inline void PathClientFoldDword(unsigned int* hash, unsigned int v)
{
    unsigned int hi = v >> 16;
    *hash += PathClientByteFold((hi >> 8) & 0xFFu);
    unsigned int folded = PathClientByteFold((v >> 8) & 0xFFu);
    folded += PathClientByteFold(v & 0xFFu);
    folded += PathClientByteFold(hi & 0xFFu);
    *hash += folded;
}

//  0x6F49CDE0 - vtable slot 15, ComputeChecksum.  This class's own base
//  contribution (NTempest::CPresenceTagged::FoldLinkFieldsIntoHash) first,
//  then m_position/m_target/m_goal (six CFloat dwords in field-declaration
//  order), then both `m_cache` entries in turn - each one's own m_count,
//  then every cached point's own x/y, then that entry's own m_cursor word
//  and its own half of the m_sizeClass/m_laneKey pair - and finally
//  m_flags/m_cooldown/m_retries/m_typeMask/m_radius in strict offset
//  order.  `m_lanePrev`/`m_laneNext`/`m_bestHandle`/`m_bestTypeTag`/
//  `m_ignore`/`m_targetNode` are genuinely not read here.
void CPathClient::ComputeChecksum(unsigned int* hash)
{
    ((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash);

    PathClientFoldDword(hash, m_position.m_x.m_bits);
    PathClientFoldDword(hash, m_position.m_y.m_bits);
    PathClientFoldDword(hash, m_target.m_x.m_bits);
    PathClientFoldDword(hash, m_target.m_y.m_bits);
    PathClientFoldDword(hash, m_goal.m_x.m_bits);
    PathClientFoldDword(hash, m_goal.m_y.m_bits);

    for (unsigned int i = 0; i < 2; i++)
    {
        PathClientFoldDword(hash, m_cache[i].m_count);

        CGridVec2* points = m_cache[i].Points();
        for (unsigned int j = 0; j < m_cache[i].m_count; j++)
        {
            PathClientFoldDword(hash, points[j].m_x.m_bits);
            PathClientFoldDword(hash, points[j].m_y.m_bits);
        }

        PathClientFoldDword(hash, m_cursor[i]);
        PathClientFoldDword(hash, (i == 0) ? m_sizeClass : m_laneKey);
    }

    PathClientFoldDword(hash, m_flags);
    PathClientFoldDword(hash, m_cooldown);
    PathClientFoldDword(hash, m_retries);
    PathClientFoldDword(hash, m_typeMask);
    PathClientFoldDword(hash, m_radius.m_bits);
}

//  0x6F49CB70 - vtable slot 16, DumpDebugState.  Same "[PrTg] t[%u] b[%u]"
//  header line as every sibling in this family, then a real virtual
//  dispatch back through this object's own slot 15 (+0x3C) to get the
//  checksum for the "[PrPa] - csum[0x%08X]" line - not an inlined call to
//  ComputeChecksum above, matching the dump's own indirect `call edx`.
//  Then one "\tpos[%u] lim[%u]" line per `m_cursor`/`m_sizeClass`-or-
//  `m_laneKey` pair, six FormatCFloat calls for m_goal/m_target/m_position
//  (reverse field order, matching every sibling's own swapped-argument
//  convention), m_flags/m_bestHandle/m_bestTypeTag, then
//  m_radius/m_typeMask.
void CPathClient::DumpDebugState(void* sink)
{
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrTg] t[%u] b[%u]", m_tagT, m_tagB);

    typedef void (__thiscall *ComputeChecksumFn)(void*, unsigned int*);
    ComputeChecksumFn fn = ((ComputeChecksumFn*)(*(void***)this))[15];
    unsigned int csum = 0;
    fn(this, &csum);
    LogFormatted(sink, "");
    LogFormatted(sink, "[PrPa] - csum[0x%08X]", csum);

    //  Tried both declaration orders (cursor-then-lim, here; lim-then-
    //  cursor, the prior session's own version) chasing the shipped
    //  stream's own register pairing (`ebp`=m_cursor, `ebx`=
    //  m_sizeClass/m_laneKey - this build has them the other way around
    //  either way) - no difference in the generated code either order,
    //  so this is cosmetic; left in this order since it reads no worse.
    //  Register allocation only, pending angr IDENTICAL verification -
    //  see this file's own header comment.
    unsigned int* cursor = m_cursor;
    unsigned short* lim = &m_sizeClass;
    for (unsigned int i = 0; i < 2; i++)
    {
        LogFormatted(sink, "\tpos[%u] lim[%u]", *cursor, *lim);
        cursor++;
        lim++;
    }

    LogFormatted(sink, "\tgol x[%s] y[%s] - x[%s] y[%s] - x[%s] y[%s]",
                 FormatCFloat(&m_position.m_x, 1, -1),
                 FormatCFloat(&m_position.m_y, 1, -1),
                 FormatCFloat(&m_target.m_x, 1, -1),
                 FormatCFloat(&m_target.m_y, 1, -1),
                 FormatCFloat(&m_goal.m_x, 1, -1),
                 FormatCFloat(&m_goal.m_y, 1, -1));
    LogFormatted(sink, "\topt[0x%08X] rcr[%u] tcn[%u]",
                 m_flags, m_cooldown, m_retries);
    LogFormatted(sink, "\tifr[%s] pfm[0x%08X]",
                 FormatCFloat(&m_radius, 1, -1), m_typeMask);
}
