//============================================================================
//  Struct declarations shared by the "checksum-cache" chain reached from
//  CCustomData::ComputeChecksum (Item/abilitycustomdata_checksum.cpp)
//  through its own m_pPool/m_ownedRef fields - see
//  Misc/poolchecksumcachehash.cpp's own file header for why neither owning
//  class is named, and why every method below is declared here but
//  defined in its own separate .cpp: each is a real, separately-addressed
//  function in the shipped binary, and a definition visible in the same TU
//  as its only caller is a standing invitation for this compiler's /Ob2 to
//  inline it away - which happened here on the first pass (all four of
//  OwnedRefChecksumCacheHash/FoldGroup/FoldRecord/NormalizeCompare living
//  in one file collapsed three real `call`s into one, dropping
//  OwnedRefChecksumCacheHash's own score from a clean structural match to
//  0.256) - the same lesson Item/abilitycustomdata_dtor.cpp's own header
//  already documents for ~CCustomData()/ReleaseOwnedRefAndFinalize().
//============================================================================
#ifndef CUSTOMDATACHECKSUMCACHE_H
#define CUSTOMDATACHECKSUMCACHE_H

//----------------------------------------------------------------------------
//  m_pPool's own checksum cache - see poolchecksumcachehash.cpp.
//----------------------------------------------------------------------------
struct SPoolChecksumCacheElem
{
    void* m_collection;         // +0x00
    char  m_reserved04[0x104];  // pad to the real stride, 0x108
};

struct SPoolChecksumCache
{
    char         m_reserved00[4];    // +0x00, never read by this function
    void*        m_collection;       // +0x04
    char         m_reserved08[0x108];
    unsigned int m_elemCount;        // +0x110
    void*        m_elemBase;         // +0x114 - SPoolChecksumCacheElem[m_elemCount]
    char         m_reserved118[0x198 - 0x118];
    unsigned int m_dirty;            // +0x198
    unsigned int m_cachedHash;       // +0x19C
};

//  0x6F6F8CD0.
unsigned int __fastcall PoolChecksumCacheHash(void* cacheVoid);

//----------------------------------------------------------------------------
//  One tagged field record (stride 0x418) - see
//  ownedrefchecksumcachefoldrecord.cpp.
//----------------------------------------------------------------------------
struct SFieldRecord
{
    unsigned int m_seedA;       // +0x00 - also the case-3 compare key
    unsigned int m_type;        // +0x04 - switch selector, 0-3
    unsigned int m_extA;        // +0x08 - only folded if cache->QueryExtFields(2)
    unsigned int m_extB;        // +0x0C - only folded if cache->QueryExtFields(2)
    union
    {
        unsigned int m_intValue;  // +0x10 - case 0/1
        char         m_string[1]; // +0x10 - case 3, a nul-terminated string
    };
    char m_reserved14[0x3FC];
    int  m_floatBits;           // +0x410 - case 2, a CFloat's raw bits
    char m_reserved414[4];
};

//  0x6F6F3820 - the referent object case 3's own vtable slot 2 lookup
//  returns; a distinct receiver from the cache/record above.  See
//  fieldrecordcompareref.cpp.
struct SFieldRecordCompareRef
{
    int __thiscall NormalizeCompare(unsigned int key);
};

//----------------------------------------------------------------------------
//  m_ownedRef's own checksum cache - see ownedrefchecksumcachehash.cpp,
//  ownedrefchecksumcachefoldgroup.cpp and
//  ownedrefchecksumcachefoldrecord.cpp.
//----------------------------------------------------------------------------
struct SOwnedRefChecksumCache
{
    unsigned int __thiscall FoldGroup(void* group);
    unsigned int __thiscall FoldRecord(SFieldRecord* record);

    char         m_reserved00[8];    // +0x00 - vtable ptr; +0x04 unread here
    unsigned int m_group1Count;      // +0x08
    void*        m_group1Base;       // +0x0C - stride 0x18
    char         m_reserved10[0x18 - 0x10];
    unsigned int m_group2Count;      // +0x18
    void*        m_group2Base;       // +0x1C - stride 0x18
    char         m_reserved20[0x30 - 0x20];
    unsigned int m_dirty;            // +0x30
    unsigned int m_cachedHash;       // +0x34
};

//  0x6F701A00.
unsigned int __fastcall OwnedRefChecksumCacheHash(void* cacheVoid);

#endif
