//============================================================================
//  0x6F701A00 - m_ownedRef's own checksum cache (CCustomData::ComputeChecksum,
//  Item/abilitycustomdata_checksum.cpp): a differently-shaped dirty-flag/
//  cached-hash pair (+0x30/+0x34) guarding two arrays of "group" records
//  (+0x08 count/+0x0C base and +0x18 count/+0x1C base, stride 0x18 either
//  way), each group folded through SOwnedRefChecksumCache::FoldGroup
//  (ownedrefchecksumcachefoldgroup.cpp - its own translation unit, not
//  this one, for the reason customdatachecksumcache.h's own header gives).
//
//  When m_dirty is already clear the shipped body skips straight to the
//  return with no register save/restore at all (`je` jumps past both the
//  `push ebx`/`push edi` at entry and their matching `pop`s at exit) - a
//  genuinely separate, simpler path, not a guard clause inside the same
//  one, which is why this reads as `if (dirty) { ...; dirty = 0; }
//  return cachedHash;` rather than an early `if (!dirty) return ...;`.
//============================================================================
#include "customdatachecksumcache.h"

unsigned int __fastcall OwnedRefChecksumCacheHash(void* cacheVoid)
{
    SOwnedRefChecksumCache* cache = (SOwnedRefChecksumCache*)cacheVoid;

    if (cache->m_dirty)
    {
        cache->m_cachedHash = 0;

        //  Both loops are plain `for`s (poolchecksumcachehash.cpp's own
        //  header note has the "guarded do/while scored worse" finding
        //  this reuses), and both re-read their own array base off
        //  `cache` fresh every iteration (an accumulated byte offset, not
        //  a hoisted element pointer) for the same reason.
        for (unsigned int i = 0, offset = 0; i < cache->m_group1Count; ++i, offset += 0x18)
        {
            void* group = (char*)cache->m_group1Base + offset;
            unsigned int hash = cache->FoldGroup(group) ^ cache->m_cachedHash;
            cache->m_cachedHash = (hash << 3) | (hash >> 29);
        }

        for (unsigned int i = 0, offset = 0; i < cache->m_group2Count; ++i, offset += 0x18)
        {
            void* group = (char*)cache->m_group2Base + offset;
            unsigned int hash = cache->FoldGroup(group) ^ cache->m_cachedHash;
            cache->m_cachedHash = (hash << 3) | (hash >> 29);
        }

        cache->m_dirty = 0;
    }

    return cache->m_cachedHash;
}
