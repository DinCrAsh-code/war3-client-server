//============================================================================
//  0x6F497ED0 - save the +0x30 link array (SAStarLinkArray, pathastar.h):
//  its own five CMemBlock/array-header dwords (m_blockSize, m_size,
//  m_growChunk, m_capacity, m_count, in that order - NOT m_block or m_data,
//  which are transient), then every live element's own three dwords
//  (SAStarLink::m_flags/m_x/m_y).  `arrayRaw` arrives untyped (`edx`, not
//  `ecx`) rather than as a real `this` - a free __fastcall taking the
//  object, the same shape WritePathRefEntry's own `raw` parameter is,
//  because the caller (CPathAStar::Save, pathastar_save.cpp) reaches it
//  through a raw `this+0x30` offset rather than a named member (see
//  pathastar.h's own header comment on SAStarLinkArray for why).
//
//  The element loop re-reads the array's own data pointer (local +0x0C)
//  every iteration rather than hoisting it once, the same byte-offset walk
//  SPathRefArray::Save's own header note already documents for this image's
//  codegen.
//============================================================================
#include "pathastar.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall SaveLinkArray(CDataStoreScratch* store,
                                             const void* arrayRaw)
{
    const unsigned int* array = (const unsigned int*)arrayRaw;

    store->WriteDword(array[2]);   // m_blockSize  (local +0x08)
    store->WriteDword(array[4]);   // m_size       (local +0x10)
    store->WriteDword(array[5]);   // m_growChunk  (local +0x14)
    store->WriteDword(array[6]);   // m_capacity   (local +0x18)
    store->WriteDword(array[7]);   // m_count      (local +0x1C)

    unsigned int i = 0;
    if (i < array[7])
    {
        unsigned int offset = 0;
        do
        {
            const unsigned char* data =
                *(const unsigned char* const*)((const char*)arrayRaw + 0x0C);
            const unsigned int* elem = (const unsigned int*)(data + offset);
            store->WriteDwordAlt2(elem[0]);
            store->WriteDwordAlt2(elem[1]);
            store->WriteDwordAlt2(elem[2]);
            offset += 0x0C;
            i++;
        } while (i < array[7]);
    }
    return store;
}
