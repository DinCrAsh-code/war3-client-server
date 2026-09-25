//============================================================================
//  0x6F6F8CD0 - m_pPool's own checksum cache (CCustomData::ComputeChecksum,
//  Item/abilitycustomdata_checksum.cpp): a dirty flag/cached-hash pair
//  (+0x198/+0x19C) guarding a fold of one direct collection (+0x04) plus an
//  array of further collections (+0x110 count, +0x114 base, stride 0x108,
//  the collection pointer itself at the element's own +0x00) - every one of
//  them folded through the SAME HashTaggedFieldRecordCollectionAlias walker
//  CAbilityDatabase::GetChecksum uses, confirming it really is the
//  class-agnostic helper its own header note already says it is.
//
//  Neither owning class is named anywhere this repo has looked - see
//  customdatachecksumcache.h's own header for why.  checksum-batch-B
//  (2026-09-12).
//============================================================================
#include "customdatachecksumcache.h"

//  0x6F4C9050 - Misc/genericfieldrecordhash.cpp.  Declared, not defined,
//  here, same convention Item/abilitydatabase_checksum.cpp already uses.
extern unsigned int __fastcall HashTaggedFieldRecordCollectionAlias(void* collection);

unsigned int __fastcall PoolChecksumCacheHash(void* cacheVoid)
{
    SPoolChecksumCache* cache = (SPoolChecksumCache*)cacheVoid;

    if (cache->m_dirty)
    {
        cache->m_cachedHash = HashTaggedFieldRecordCollectionAlias(cache->m_collection);

        //  A plain `for` (this compiler tests the count once, jumps to the
        //  bottom-of-loop check, and falls into the body - the dump's own
        //  `jbe skip / jmp ->+2` past a 16-byte alignment pad is exactly
        //  that shape, not a real second branch) - `cache->m_elemBase` is
        //  re-read off `cache` fresh every iteration (never hoisted into a
        //  register-held local across the loop) for the same
        //  "the callee could reach back and change it" reason
        //  HashTaggedFieldRecordCollection's own header note gives for its
        //  vtable re-read, since HashTaggedFieldRecordCollectionAlias is an
        //  indirect-call-carrying walker this compiler cannot prove leaves
        //  `cache` alone.  (A guarded `do`/`while` was tried first and
        //  scored worse, 17/32 vs this shape's 28/32 - the compiler's own
        //  register allocation for a `for` happened to already match.)
        for (unsigned int i = 0, offset = 0; i < cache->m_elemCount; ++i, offset += sizeof(SPoolChecksumCacheElem))
        {
            SPoolChecksumCacheElem* elem =
                (SPoolChecksumCacheElem*)((char*)cache->m_elemBase + offset);
            unsigned int hash = cache->m_cachedHash ^ HashTaggedFieldRecordCollectionAlias(elem->m_collection);
            cache->m_cachedHash = (hash << 3) | (hash >> 29);
        }

        cache->m_dirty = 0;
    }

    return cache->m_cachedHash;
}
