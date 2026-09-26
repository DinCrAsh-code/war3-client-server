#include <stdlib.h>

#include "Engine.h"

// Size-class pool allocator: small blocks are carved out of 64K pages and
// recycled through per-class free lists; large blocks go straight to malloc.

namespace
{
    const u16 kBlockMagic    = 0x4D52;
    const u8  kLargeClass    = 0xFF;
    const u32 kPageSize      = 0x10000;
    const int kNumClasses    = 13;

    const u16 s_classSizes[kNumClasses] =
    {
        16, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 2048
    };

    struct BlockHeader
    {
        u16 magic;
        u8  sizeClass;
        u8  flags;
        u32 size;
    };

    struct FreeBlock
    {
        FreeBlock* next;
    };

    struct PageHeader
    {
        PageHeader* next;
        u32         sizeClass;
    };

    FreeBlock*  s_freeLists[kNumClasses];
    PageHeader* s_pages;
    u32         s_bytesInUse;
    u32         s_pageCount;
    bool        s_initialized;

    int SizeClassFor(u32 size)
    {
        int lo = 0;
        int hi = kNumClasses - 1;
        if (size > s_classSizes[hi])
            return -1;
        while (lo < hi)
        {
            int mid = (lo + hi) >> 1;
            if (s_classSizes[mid] < size)
                lo = mid + 1;
            else
                hi = mid;
        }
        return lo;
    }

    bool RefillClass(int sizeClass)
    {
        u8* page = static_cast<u8*>(malloc(kPageSize));
        if (!page)
            return false;

        PageHeader* ph = reinterpret_cast<PageHeader*>(page);
        ph->next = s_pages;
        ph->sizeClass = (u32)sizeClass;
        s_pages = ph;
        ++s_pageCount;

        u32 stride = s_classSizes[sizeClass] + sizeof(BlockHeader);
        u32 offset = (sizeof(PageHeader) + 7) & ~7u;
        while (offset + stride <= kPageSize)
        {
            FreeBlock* block = reinterpret_cast<FreeBlock*>(page + offset);
            block->next = s_freeLists[sizeClass];
            s_freeLists[sizeClass] = block;
            offset += stride;
        }
        return true;
    }
}

void MemInit()
{
    if (s_initialized)
        return;
    for (int i = 0; i < kNumClasses; ++i)
        s_freeLists[i] = 0;
    s_pages = 0;
    s_bytesInUse = 0;
    s_pageCount = 0;
    s_initialized = true;
}

void MemShutdown()
{
    PageHeader* page = s_pages;
    while (page)
    {
        PageHeader* next = page->next;
        free(page);
        page = next;
    }
    s_pages = 0;
    for (int i = 0; i < kNumClasses; ++i)
        s_freeLists[i] = 0;
    s_initialized = false;
}

void* MemAlloc(u32 size)
{
    if (!s_initialized)
        MemInit();
    if (size == 0)
        size = 1;

    BlockHeader* header;
    int sizeClass = SizeClassFor(size);
    if (sizeClass < 0)
    {
        header = static_cast<BlockHeader*>(malloc(size + sizeof(BlockHeader)));
        if (!header)
            return 0;
        header->sizeClass = kLargeClass;
    }
    else
    {
        if (!s_freeLists[sizeClass] && !RefillClass(sizeClass))
            return 0;
        FreeBlock* block = s_freeLists[sizeClass];
        s_freeLists[sizeClass] = block->next;
        header = reinterpret_cast<BlockHeader*>(block);
        header->sizeClass = (u8)sizeClass;
    }

    header->magic = kBlockMagic;
    header->flags = 0;
    header->size = size;
    s_bytesInUse += size;

    void* user = header + 1;
    memset(user, 0, size);
    return user;
}

void MemFree(void* p)
{
    if (!p)
        return;
    BlockHeader* header = static_cast<BlockHeader*>(p) - 1;
    if (header->magic != kBlockMagic)
        return;

    header->magic = 0;
    s_bytesInUse -= header->size;
    if (header->sizeClass == kLargeClass)
    {
        free(header);
        return;
    }

    // The free-list link overlays the header, so read the class first.
    int sizeClass = header->sizeClass;
    FreeBlock* block = reinterpret_cast<FreeBlock*>(header);
    block->next = s_freeLists[sizeClass];
    s_freeLists[sizeClass] = block;
}

void* MemRealloc(void* p, u32 size)
{
    if (!p)
        return MemAlloc(size);

    BlockHeader* header = static_cast<BlockHeader*>(p) - 1;
    if (header->sizeClass != kLargeClass && size <= s_classSizes[header->sizeClass])
    {
        s_bytesInUse += size;
        s_bytesInUse -= header->size;
        header->size = size;
        return p;
    }

    void* fresh = MemAlloc(size);
    if (!fresh)
        return 0;
    memcpy(fresh, p, header->size < size ? header->size : size);
    MemFree(p);
    return fresh;
}

// ---------------------------------------------------------------------------
// Handle table: handles are (generation << 16) | (slot + 1), so a stale
// handle to a recycled slot fails the generation check instead of aliasing
// a new object.

void CHandleTable::Init(int capacity)
{
    m_capacity = capacity;
    m_slots = static_cast<Slot*>(MemAlloc((u32)(capacity * sizeof(Slot))));
    for (int i = 0; i < capacity; ++i)
    {
        m_slots[i].object = 0;
        m_slots[i].generation = 1;
        m_slots[i].nextFree = (u16)(i + 1);
    }
    m_freeHead = 0;
    m_liveCount = 0;
}

void CHandleTable::Shutdown()
{
    MemFree(m_slots);
    m_slots = 0;
    m_capacity = 0;
}

Handle CHandleTable::Register(CObject* obj)
{
    if (m_freeHead >= m_capacity)
        return 0;

    int index = m_freeHead;
    Slot& slot = m_slots[index];
    m_freeHead = slot.nextFree;
    slot.object = obj;
    ++m_liveCount;

    Handle h = ((Handle)slot.generation << 16) | (Handle)(index + 1);
    obj->m_handle = h;
    return h;
}

void CHandleTable::Unregister(Handle h)
{
    int index = (int)(h & 0xFFFF) - 1;
    if (index < 0 || index >= m_capacity)
        return;
    Slot& slot = m_slots[index];
    if (slot.generation != (u16)(h >> 16) || !slot.object)
        return;

    slot.object = 0;
    slot.generation = (u16)(slot.generation + 1);
    if (slot.generation == 0)
        slot.generation = 1;
    slot.nextFree = (u16)m_freeHead;
    m_freeHead = index;
    --m_liveCount;
}

CObject* CHandleTable::Lookup(Handle h) const
{
    int index = (int)(h & 0xFFFF) - 1;
    if (index < 0 || index >= m_capacity)
        return 0;
    const Slot& slot = m_slots[index];
    if (slot.generation != (u16)(h >> 16))
        return 0;
    return slot.object;
}
