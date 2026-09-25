//============================================================================
//  FACEDATA's kerning-pair bucket table
//  (`TSHashTable<KERNNODE,KERNINGHASHKEY>`-shaped, Render/facedata.h) - its
//  constructor, destructor and Clear.  Same shape as
//  Render/facechartable.cpp's CHARCODEDESC_TABLE; see that file for why
//  Clear()'s vtable-slot-0 call is a raw indirect call rather than a
//  `virtual` declaration, and why the destructor's frame does not
//  byte-match.
//============================================================================
#include "facedata.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UKERNNODE@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

//  0x6F7C38F0
KERNNODE_TABLE::KERNNODE_TABLE()
    : m_vtable((void*)kKernNodeTableVftable)
{
    m_buckets.m_alloc = 0;
    m_buckets.m_count = 0;
    m_buckets.m_data = 0;
    m_buckets.m_chunk = 0;
    m_probes = 0;
    m_list.SetLinkOffset(0x0C);
    m_mask = 0xFFFFFFFFu;
}

//  0x6F7C3B30
KERNNODE_TABLE::~KERNNODE_TABLE()
{
    m_vtable = (void*)kKernNodeTableVftable;
    Clear(1);
}

//  0x6F7C0190 - `retn 4`, thiscall + one stack bool.
void KERNNODE_TABLE::Clear(unsigned int freeNodes)
{
    m_probes = 0;
    m_list.UnlinkAll();

    typedef void (__thiscall *FreeNodeFn)(void*, void*);

    for (unsigned int i = 0; i < m_buckets.m_count; i++)
    {
        TSExplicitList<KERNNODE>& bucket = m_buckets[i];
        for (;;)
        {
            int tail = bucket.TailLink();
            if (tail <= 0)
                break;
            if (freeNodes)
                bucket.Link((KERNNODE*)tail)->Unlink();
            else
                ((FreeNodeFn)(*(void***)this)[0])(this, (void*)tail);
        }
    }
}

//  0x6F7BF130
template void TSExplicitList<KERNNODE>::UnlinkAll();

//  0x6F7C24C0
template TSGrowableArray<TSExplicitList<KERNNODE> >::~TSGrowableArray();
