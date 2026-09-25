//============================================================================
//  ActionQueueRecord's own fixed-slab free list: a chunked array of
//  (this+0x14 .. this+0x814, i.e. 0x800 bytes) slots, each `arg+8` bytes,
//  threaded as a singly-linked free list through each slot's own first
//  field (its second field always points back at the record). this->0x8
//  is the record's own "current slot size" and this->0xC/this->0x10 are a
//  count and the free-list head. Reached from
//  ServerTock_0x27_PacketHandler's own SEH-protected relay builders
//  (sub_6F6690F0/sub_6F6691F0) and from the root handler itself, all in the
//  0x6F663xxx neighbourhood.
//
//  Declared as methods of a minimal partial-layout class (raw offset casts
//  for everything except the three touched fields) purely so the compiler
//  emits `this` in ecx the way the shipped `ecx`-as-self free functions do -
//  no wider ActionQueueRecord layout is asserted here.
//============================================================================
class ActionQueueRecordFreeList
{
public:
    //  0x6F663BD0 - (re)carves the whole 0x800-byte slab into `arg`-sized
    //  slots and rebuilds the free list from scratch, discarding whatever
    //  was linked before. Reproduces the shipped code's own apparent latent
    //  bug literally: if a single slot (arg+8 bytes) does not even fit once
    //  in the slab, the shipped code writes through a null pointer instead
    //  of leaving the free list empty - unreachable in practice (this->0x8
    //  slot sizes never approach 0x800), but the instructions are what they
    //  are, and "fix" was never on offer per CLAUDE.md's own rule against
    //  changing what the shipped code does.
    void GrowFreeList(unsigned int arg);

    //  0x6F663D70 - pop a free slot: if `arg` differs from the record's
    //  current slot size, either bail out empty-handed (m_count is still
    //  nonzero - some other slab is still in use) or rebuild the whole slab
    //  for the new size via GrowFreeList; then pop the list head, bumping
    //  m_count.
    void* AllocateFreeNode(unsigned int arg);

    //  0x6F663BB0 - push a slot back onto the free list, decrementing
    //  m_count.
    void ReleaseFreeNode(void* node);

    char           m_reserved00[8];
    unsigned int   m_slotSize;   // +0x08
    int            m_count;      // +0x0C
    void*          m_freeHead;   // +0x10
    char           m_slab[0x800];// +0x14
};

void ActionQueueRecordFreeList::GrowFreeList(unsigned int arg)
{
    char* base = m_slab;
    char* firstNext = base + arg + 8;
    char* end = m_slab + sizeof(m_slab);
    m_slotSize = arg;

    if (firstNext <= end)
    {
        unsigned int step = arg + 8;
        char* p = base;
        char* next = firstNext;
        do
        {
            *(void**)p = next;
            *(void**)(p + 4) = this;
            p = next;
            next = p + step;
        } while (next <= end);
        *(void**)p = 0;
    }
    else
    {
        *(unsigned int*)(void*)0 = 0;
    }

    m_freeHead = base;
}

void* ActionQueueRecordFreeList::AllocateFreeNode(unsigned int arg)
{
    if (arg != m_slotSize)
    {
        if (m_count == 0)
            GrowFreeList(arg);
        else
            return 0;
    }

    void* head = m_freeHead;
    if (head == 0)
        return 0;

    void* next = *(void**)head;
    m_count += 1;
    m_freeHead = next;
    return head;
}

void ActionQueueRecordFreeList::ReleaseFreeNode(void* node)
{
    void* oldHead = m_freeHead;
    m_count -= 1;
    *(void**)node = oldHead;
    m_freeHead = node;
}
