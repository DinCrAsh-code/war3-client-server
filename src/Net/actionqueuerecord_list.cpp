//============================================================================
//  ActionQueueRecordList - a two-field (m_next at +0, m_prevlink at +4)
//  intrusive doubly-linked list using the exact same complemented-terminator
//  encoding storm.h's own TSLink<T>::Unlink() documents (m_prevlink <= 0
//  means "this is the terminator's own complemented address", > 0 means "a
//  real predecessor object pointer") - not an instantiation of that
//  template (this class's own terminator is `this+4` directly, not a
//  separate TSLink member, and the insert half below has no TSExplicitList
//  counterpart), but unmistakably the same hand-rolled link convention.
//============================================================================

class ActionQueueRecordList
{
public:
    //  0x6F668330 - `link ? link->m_next : this->m_default` accessor.
    void* GetLinkOrDefault(void* link);

    //  0x6F668290 - unlink `unlinkArg` (or this list's own root, `this+4`,
    //  if null) from wherever it currently sits, then relink it either
    //  immediately after `targetList` (or this list's own root) when
    //  `insertMode == 1`, or by walking `insertCount - 1` steps first.
    void Relink(void* unlinkArg, unsigned int insertCount, void* targetList);

    //  0x6F66F1D0 - SMemAlloc a fresh 0x814+extraSize-byte node (tag: the
    //  RTTI type descriptor name of whatever struct this is - IDA's own
    //  display truncates it to "aAuturnssyncchu", the fuller text not
    //  reached by anything else in this call tree), zero its first five
    //  dwords, and - unless `linkArg` is null - Relink it into this list at
    //  the position `linkArg` names. Returns the new node, or null if the
    //  allocation itself failed (Relink is still skipped either way when
    //  linkArg is null, matching the shipped code's own order: the null
    //  check on `linkArg` happens after the alloc regardless of whether it
    //  succeeded).
    void* AllocateAndLink(void* linkArg, unsigned int extraSize, unsigned int flags);
};

void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);

void* ActionQueueRecordList::GetLinkOrDefault(void* link)
{
    if (link)
        return *(void**)((char*)link + 4);
    return *(void**)((char*)this + 8);
}

void ActionQueueRecordList::Relink(void* unlinkArg, unsigned int insertCount, void* targetList)
{
    void* eax = unlinkArg ? unlinkArg : (void*)((char*)this + 4);

    void* edi = *(void**)eax;
    if (edi != 0)
    {
        void* esi = *(void**)((char*)eax + 4);
        if ((int)esi > 0)
            esi = (void*)((int)esi + ((int)eax - *(int*)((char*)edi + 4)));
        else
            esi = (void*)~(int)esi;
        *(void**)esi = edi;

        void* edx = *(void**)eax;
        void* esi2 = *(void**)((char*)eax + 4);
        *(void**)((char*)edx + 4) = (void*)(int)esi2;
        *(void**)eax = 0;
        *(void**)((char*)eax + 4) = 0;
    }

    void* edx = targetList ? targetList : (void*)((char*)this + 4);

    if (insertCount - 1 != 0)
    {
        void* ecx = *(void**)edx;
        *(void**)eax = ecx;
        *(void**)((char*)eax + 4) = *(void**)((char*)ecx + 4);
        *(void**)((char*)ecx + 4) = unlinkArg;
        *(void**)edx = eax;
        return;
    }

    *(void**)eax = edx;
    void* esi = *(void**)((char*)edx + 4);
    *(void**)((char*)eax + 4) = esi;
    void* self = this;
    void* ecx = *(void**)((char*)edx + 4);
    if ((int)esi >= 0)
    {
        void* edi2 = *(void**)edx;
        esi = (void*)((int)edx - *(int*)((char*)edi2 + 4));
        ecx = (void*)((int)ecx + (int)esi);
        *(void**)ecx = eax;
    }
    else
    {
        ecx = (void*)~(int)ecx;
        *(void**)ecx = eax;
    }
    (void)self;
    *(void**)((char*)edx + 4) = unlinkArg;
}

void* ActionQueueRecordList::AllocateAndLink(void* linkArg, unsigned int extraSize, unsigned int flags)
{
    void* node = SMemAlloc(extraSize + 0x814, "AutoTurnSyncChunk", -2, flags | 8);
    void* result;
    if (node != 0)
    {
        *(int*)((char*)node + 0) = 0;
        *(int*)((char*)node + 4) = 0;
        *(int*)((char*)node + 8) = 0;
        *(int*)((char*)node + 0xC) = 0;
        *(int*)((char*)node + 0x10) = 0;
        result = node;
    }
    else
    {
        result = 0;
    }

    if (linkArg != 0)
        Relink(result, (unsigned int)linkArg, 0);

    return result;
}
