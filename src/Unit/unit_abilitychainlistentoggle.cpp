//============================================================================
//  0x6F28E2C0 - reached from CUnit::NotifyAbilityHostChanged
//  (unit_abilityhostnotify.cpp) as `Notify_6F28E2C0(1)`, the first time
//  only (its own +0x5C bit 4 guard).  `this` in ecx, one stack bool
//  (`activate`), `retn 4`.
//
//  Walks the same +0x1DC ability chain CUnit::NotifyAbilitiesOfMessage
//  (0x6F27BA70, unit_notifyabilitieschain.cpp) already walks - each entry
//  a QueryHandleField0x54-resolved node whose own +0x24 handle pair is the
//  chain's "next" link - but instead of dispatching a message through
//  slot 0x100, it toggles each node's own +0x20 bit 1 (0x2) and, only when
//  the node's own +0x3C gate is not positive, calls through slot 0xC8
//  (`activate` true) or 0xC4 (`activate` false).  Two full, separately
//  compiled copies of the walk - one per branch, not one loop re-testing
//  `activate` every iteration - which is what the shipped stream itself
//  shows (see the diff notes on the CSE below).
//
//  `(ref->m_handle & ref->m_typeTag)` is computed once, ahead of the
//  `activate` branch, and each branch's own first `cmp eax, -1` reads that
//  same register - the shipped compiler hoisted the common subexpression
//  out of both duplicated branches rather than recomputing it in each.
//============================================================================
#include "unit.h"
#include "../Item/itemhandleresolve.h"

typedef void (__thiscall *ListenerToggleFn)(void*);

void CUnit::NotifyAbilityChainListenerToggle(int activate)
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)&m_ref1DC;
    unsigned int gate = ref->m_handle & (unsigned int)ref->m_typeTag;

    if (activate)
    {
        if (gate == 0xFFFFFFFFu)
            return;
        void* node = QueryHandleField0x54(ref);
        if (!node)
            return;

        do
        {
            if (*(int*)((char*)node + 0x3C) <= 0)
            {
                void** vtbl = *(void***)node;
                *(volatile unsigned int*)((char*)node + 0x20) |= 2;
                ListenerToggleFn fn = (ListenerToggleFn)vtbl[0xC8 / 4];
                fn(node);
            }

            ref = (SOptionalHandleRef*)((char*)node + 0x24);
            node = ((ref->m_handle & (unsigned int)ref->m_typeTag) == 0xFFFFFFFFu)
                       ? 0 : QueryHandleField0x54(ref);
        } while (node);
    }
    else
    {
        if (gate == 0xFFFFFFFFu)
            return;
        void* node = QueryHandleField0x54(ref);
        if (!node)
            return;

        do
        {
            if (*(int*)((char*)node + 0x3C) <= 0)
            {
                void** vtbl = *(void***)node;
                *(volatile unsigned int*)((char*)node + 0x20) &= ~2u;
                ListenerToggleFn fn = (ListenerToggleFn)vtbl[0xC4 / 4];
                fn(node);
            }

            ref = (SOptionalHandleRef*)((char*)node + 0x24);
            node = ((ref->m_handle & (unsigned int)ref->m_typeTag) == 0xFFFFFFFFu)
                       ? 0 : QueryHandleField0x54(ref);
        } while (node);
    }
}
