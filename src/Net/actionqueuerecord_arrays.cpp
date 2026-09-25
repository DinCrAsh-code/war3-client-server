//============================================================================
//  Two parallel byte-keyed linear-search-then-erase arrays on
//  ActionQueueRecord (0x6F663xxx neighbourhood, same module as
//  actionqueuerecord_freelist.cpp's own slab allocator): the first rooted
//  at this+4 with 0xC-byte elements and its own count at this+0xC8, the
//  second rooted at this+4 with a this+0xC4-relative stride (0xC bytes,
//  computed as base*3) and its own count at this+0xCC. Each array's own
//  Find (FindAndEraseFirst/Second, actionqueuerecord_arrays_find.cpp) is
//  kept in a separate translation unit so the compiler cannot inline the
//  erase calls below into it - see that file's own header.
//============================================================================

class ActionQueueRecordArrays
{
public:
    //  0x6F663DA0 - shift-erase one 0xC-byte element out of the first array.
    void EraseFirst(unsigned int index);
    //  0x6F663DE0 - the second array's own twin of EraseFirst.
    void EraseSecond(unsigned int index);

    char           m_reserved00[0xC4];
    unsigned int   m_secondBase;    // +0xC4
    unsigned int   m_firstCount;    // +0xC8
    unsigned int   m_secondCount;   // +0xCC
};

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

void ActionQueueRecordArrays::EraseFirst(unsigned int index)
{
    char* base = (char*)this;
    char* elem = base + 4 + index * 0xC;
    unsigned int trailing = (m_firstCount - index - 1) * 0xC;
    memmove(elem + 4, elem + 0x10, trailing);
    m_firstCount -= 1;
}

void ActionQueueRecordArrays::EraseSecond(unsigned int index)
{
    unsigned int elemOffset = (m_secondBase + index) * 3 * 4;
    char* elem = (char*)this + elemOffset;
    unsigned int trailing = (m_secondCount - index - 1) * 0xC;
    memmove(elem + 4, elem + 0x10, trailing);
    m_secondCount -= 1;
}
