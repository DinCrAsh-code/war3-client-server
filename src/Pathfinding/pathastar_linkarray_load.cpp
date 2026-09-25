//============================================================================
//  0x6F497F60 - SaveLinkArray's own counterpart (pathastar_linkarray_save.cpp):
//  read the same five dwords back, reallocate the block to `m_size` bytes
//  with a `header = m_blockSize - m_size` (CMemBlock::Init, already
//  reconstructed, cellbuffer.cpp), then read every element's own three
//  dwords into the freshly allocated storage - same free-`__fastcall`,
//  same re-read-the-data-pointer-every-iteration shape as the save side.
//============================================================================
#include "pathastar.h"
#include "cdatastore.h"

CDataStore* __fastcall LoadLinkArray(CDataStore* store, void* arrayRaw)
{
    unsigned int* array = (unsigned int*)arrayRaw;

    store->ReadDword(&array[2]);   // m_blockSize
    store->ReadDword(&array[4]);   // m_size
    store->ReadDword(&array[5]);   // m_growChunk
    store->ReadDword(&array[6]);   // m_capacity
    store->ReadDword(&array[7]);   // m_count

    unsigned int size   = array[4];
    unsigned int header = array[2] - size;
    ((CMemBlock*)arrayRaw)->Init(size, header, 0, 0);

    unsigned int i = 0;
    if (i < array[7])
    {
        unsigned int offset = 0;
        do
        {
            unsigned char* data =
                *(unsigned char**)((char*)arrayRaw + 0x0C);
            unsigned int* elem = (unsigned int*)(data + offset);
            store->ReadDwordAlt3(&elem[0]);
            store->ReadDwordAlt3(&elem[1]);
            store->ReadDwordAlt3(&elem[2]);
            offset += 0x0C;
            i++;
        } while (i < array[7]);
    }
    return store;
}
