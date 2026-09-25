//============================================================================
//  ActionQueueTable::FindOrCreateRecord (0x6F66CEA0) - despite the name a
//  previous session gave this address (inferred only from
//  GameAction_0x26_PacketHandler's own call site, before this session
//  located this function's own dump), its real body never allocates
//  anything: on a non-null key it waits on a ConditionVariable embedded at
//  this+0x20, then walks an existing singly-linked chain rooted at this+0x34
//  comparing each node's own +0xC field against `key`; if nothing matches
//  (or key is null, or the chain is empty), it signals that same
//  ConditionVariable and reports "nothing found" (*wasCreated = -1) rather
//  than creating a record. The name is kept - GameAction_0x26_PacketHandler
//  already committed it to funcmap.py and to a finished target's own
//  write-up - but this comment documents what the body actually does.
//
//  arg4 (createIfMissing in the existing declaration) and arg8 are genuinely
//  unused: retn 0x14 pops five stack dwords but the body only ever reads
//  three of them (key, arg2, the wasCreated out-pointer).
//============================================================================
#include "conditionvariable.h"

class ActionQueueTable
{
public:
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};

void* ActionQueueTable::FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                                            unsigned int /*arg4*/, unsigned int /*createIfMissing*/)
{
    if (key != 0)
    {
        ((ConditionVariable*)((char*)this + 0x20))->Wait(arg2);

        int raw = *(int*)((char*)this + 0x34);
        void* node = raw > 0 ? (void*)raw : 0;

        while (node)
        {
            if (*(void**)((char*)node + 0xC) == key)
            {
                *wasCreated = (arg2 != 0) ? 1 : 0;
                return node;
            }
            void* next = *(void**)((char*)node + 8);
            if ((int)next <= 0)
                break;
            node = next;
        }

        ((ConditionVariable*)((char*)this + 0x20))->SignalOne(arg2);
    }

    *wasCreated = -1;
    return 0;
}
