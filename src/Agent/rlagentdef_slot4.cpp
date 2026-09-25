//============================================================================
//  0x6F4A5680 - NIpse::CRlAgentDef::Slot4, vtable slot 4;
//  0x6F4A4750 - NotifyListeners, its own private helper; and
//  0x6F46ABE0 - CDynTable_SRelationEndpointRecord::RemoveEntries (declared
//  in rlagentdef.h) - all reached only from here, so kept together the
//  same way Pathfinding/crlagent.h's own related-agents batch does.
//============================================================================
#include "rlagentdef.h"
#include "agentrelation.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F46ABE0 - see rlagentdef.h's own struct comment.  Same shape as
//  AgileAgentQuery::RemoveEntries (Agent/agentqueryentries.cpp, 0x6F46AC70)
//  with a 0x2C-byte element instead of an 8-byte one.
//----------------------------------------------------------------------------
int CDynTable_SRelationEndpointRecord::RemoveEntries(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove((char*)m_data + start * 0x2C,
                (char*)m_data + (start + count) * 0x2C, trailing * 0x2C);

    m_count -= count;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4A4750 - walk the singly-linked list of listeners rooted at +0x58,
//  calling each one's own vtable slot 4 with `arg`; the listener's own
//  slot-4 body is what advances the head (unlinking itself), not this
//  loop.  If `arg` itself carries a periodic callback at +0x24, it is
//  invoked once every 65 listeners notified (a stall-avoidance yield, not
//  anything this call tree names further).
//----------------------------------------------------------------------------
typedef void (__thiscall *ListenerNotifyFn)(void*, void*);
typedef void (__cdecl *YieldFn)();

void CRlAgentDef::NotifyListeners(void* arg)
{
    YieldFn yield = 0;
    if (arg != 0)
        yield = *(YieldFn*)((char*)arg + 0x24);

    unsigned int counter = 0;
    void* node = *(void**)((char*)this + 0x58);
    while (node != 0)
    {
        void* vtbl = *(void**)node;
        ListenerNotifyFn fn = *(ListenerNotifyFn*)((char*)vtbl + 0x10);
        fn(node, arg);

        if (yield != 0)
        {
            counter += 1;
            if (counter > 0x40)
            {
                yield();
                counter = 0;
            }
        }

        node = *(void**)((char*)this + 0x58);
    }
}

void CRlAgentDef::Slot4(int arg)
{
    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    *(unsigned int*)(self + 0x4C) |= 0x1000000;
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    NotifyListeners((void*)arg);

    unsigned int count = *(unsigned int*)(self + 0x124);
    if (count != 0)
        ((CDynTable_SRelationEndpointRecord*)(self + 0x108))->RemoveEntries(0, count);

    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
