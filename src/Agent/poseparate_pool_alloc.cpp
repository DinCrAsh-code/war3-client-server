//============================================================================
//  0x6F4958D0 - NIpse::SPoSeparatePool::Allocate. See poseparate_pool.h.
//============================================================================
#include "poseparate_pool.h"
#include "CFloat.h"

namespace NIpse {

CPoSeparate* SPoSeparatePool::Allocate(int zeroMemory)
{
    void* freeNode = m_freeListHead;
    CPoSeparate* obj;

    if (freeNode == 0)
    {
        void* raw = m_allocator.Alloc(zeroMemory, ".?AVCPoSeparate@NIpse@@", -2);
        obj = (CPoSeparate*)((char*)raw + 4);
        if (obj != 0)
        {
            char* base = (char*)obj;
            *(void**)(base + 4)  = 0;
            *(void**)(base + 8)  = 0;
            *(void**)(base + 0xC) = 0;
            *(void**)(base + 0x10) = 0;
            *(unsigned int*)(base + 0x14) = 0;
            *(void**)base = (void*)0x6F9521B4;   // ??_7CPoSeparate@NIpse@@6B@
            *(unsigned int*)(base + 0x18) = g_CFloatZero.m_bits;
            *(unsigned int*)(base + 0x1C) = g_CFloatZero.m_bits;
            *(unsigned int*)(base + 0x20) = 0;
        }
    }
    else
    {
        m_freeListHead = *(void**)freeNode;
        obj = (CPoSeparate*)((char*)freeNode + 4);
    }

    ++m_liveCount;
    return obj;
}

}  // namespace NIpse
