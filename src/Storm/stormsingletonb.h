//============================================================================
//  The small (0x10 byte) lazily-constructed Storm singleton
//  GetStormSingletonB hands back (0x6F01F760, lazysingleton.cpp).
//
//  Its four words are a TSGrowableArray's - allocated size, used count, the
//  storage and the growth chunk - which is what 0x6F0116E0 says: it walks
//  [+0x08] with [+0x04] as the bound and a 0x2C-byte stride.  Only the first
//  word of an element is reached, so the rest of the element stays a gap.
//============================================================================
#ifndef STORMSINGLETONB_H
#define STORMSINGLETONB_H

//  One registered obstruction candidate.  Only the object pointer is
//  touched; the other 0x28 bytes are never read in this closure.
struct SObstructionEntry
{
    void* m_pObject;                    // +0x00
    char  m_reserved04[0x28];           // +0x04
};

struct SStormSingletonB
{
    //  0x6F0116E0 - Storm/stormobstructionquery.cpp.
    int QueryNearestObstruction(const float* from, const float* to,
                                float* outDistance);

    //  0x6F011780 - Storm/stormgroundheight.cpp.
    void QueryGroundHeightAt(const float* point, float* outHeight);

    unsigned int       m_alloc;         // +0x00
    unsigned int       m_count;         // +0x04
    SObstructionEntry* m_data;          // +0x08
    unsigned int       m_chunk;         // +0x0C
};

SStormSingletonB* __fastcall GetStormSingletonB();      // 0x6F01F760

#endif
