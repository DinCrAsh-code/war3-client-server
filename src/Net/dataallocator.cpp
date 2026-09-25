//============================================================================
//  See dataallocator.h.
//============================================================================
#include "dataallocator.h"
#include "storm.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//----------------------------------------------------------------------------
//  0x6F4C1AB0.  `tag`/`line` are only used verbatim when m_growCount != 1 -
//  a pool that grows by exactly one element at a time (the common case)
//  always attributes its own allocations to its own source, discarding
//  whatever the caller passed; a pool that grows by more than one honours
//  the caller's tag/line for that chunk.  Every member read below is
//  re-read from `this` rather than cached across the free-list build loop,
//  matching the shipped code exactly: a `void*` write through `elem` could
//  alias any of this object's own fields as far as the compiler can prove,
//  so /O2 reloads m_elementSize and m_growCount on every iteration instead
//  of keeping either in a register.
//----------------------------------------------------------------------------
void* CDataAllocator::Alloc(int zeroMemory, const char* tag, int line)
{
    unsigned int growCount = m_growCount;
    if (growCount == 1)
    {
        tag = 0;
        line = 0;
    }

    if (m_freeListHead == 0)
    {
        if (tag == 0)
        {
            line = 0x98;
            tag = ".\\CDataAllocator.cpp";
        }

        void* block = SMemAlloc(m_elementSize * growCount + 4, tag, line, 0);

        void* elem = (char*)block + 4;
        m_freeListHead = elem;
        for (unsigned int i = 0; i < m_growCount - 1; i++)
        {
            void* next = (char*)elem + m_elementSize;
            *(void**)elem = next;
            elem = next;
        }
        *(void**)elem = 0;

        *(void**)block = m_blockListHead;
        m_blockListHead = block;
    }

    void* result = m_freeListHead;
    m_freeListHead = *(void**)result;

    if (zeroMemory)
        memset(result, 0, m_elementSize);

    m_liveCount += 1;
    return result;
}

//----------------------------------------------------------------------------
//  0x6F4C1B50.  `tag`/`line` are two stack arguments every call site in
//  this call tree pushes as 0 and never lets this function read - kept as
//  named, unused parameters rather than dropped, matching the shipped
//  `retn 0Ch` (three stack args, one read).
//----------------------------------------------------------------------------
void CDataAllocator::Free(void* ptr, const char* /*tag*/, int /*line*/)
{
    *(void**)ptr = m_freeListHead;
    m_liveCount -= 1;
    m_freeListHead = ptr;
}

//----------------------------------------------------------------------------
//  0x6F4C1A60.  See dataallocator.h.
//----------------------------------------------------------------------------
void CDataAllocator::ReleaseAllBlocks(const char* tag, int line, int /*unused*/)
{
    while (m_blockListHead != 0)
    {
        void* block = m_blockListHead;
        m_blockListHead = *(void**)block;

        const char* freeTag = tag;
        int         freeLine = line;
        if (freeTag == 0)
        {
            freeLine = 0x7F;
            freeTag = ".\\CDataAllocator.cpp";
        }

        SMemFree(block, freeTag, freeLine, 0);
    }

    m_freeListHead = 0;
    m_liveCount = 0;
}
