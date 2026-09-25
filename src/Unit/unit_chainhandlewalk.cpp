//============================================================================
//  CUnit batch-15 (slot 72 closure).  0x6F276920 - resolve `this`'s own
//  +0x1DC/+0x1E0 SOptionalHandleRef pair (itemhandleresolve.h) through
//  QueryHandleField0x54, then walk the chain that comes back: each node
//  carries its own +0x24/+0x28 pair pointing at the next one, and every
//  node in the chain gets one virtual dispatch (slot 0x170/4 = 92) with the
//  caller's own three stack arguments, unchanged from node to node.
//
//  `this`'s own owning class is not established anywhere in this call tree
//  (nothing else reaches offset 0x1DC), and the per-node class dispatched
//  through slot 0x170 has no committed vtable either - both left as raw
//  offsets/an untyped dispatch on `void*` rather than invented names, the
//  same restraint vtable_dispatch_audit.py's own "no committed vtable"
//  skip bucket exists for.  `retn 0Ch` - three stack dwords.
//============================================================================
#include "itemhandleresolve.h"

typedef void (__thiscall *ChainNodeDispatchFn)(void*, void*, void*, void*);

//  A free __thiscall function is not legal in this compiler - wrapped as a
//  one-method host the same way GameUIMember6F2F75F0Host::Call
//  (GameUI/gameuiforwardcalls.cpp) is, purely so `this` lands in ecx.
struct HandleRefChainOwner0x1DC
{
    void Walk(void* arg0, void* arg4, void* arg8);
};

void __thiscall HandleRefChainOwner0x1DC::Walk(void* arg0, void* arg4, void* arg8)
{
    void* thisPtr = this;
    unsigned int* pLow  = (unsigned int*)((char*)thisPtr + 0x1DC);
    unsigned int* pHigh = (unsigned int*)((char*)thisPtr + 0x1E0);

    if ((*pHigh & *pLow) == 0xFFFFFFFF)
        return;

    void* node = QueryHandleField0x54((SOptionalHandleRef*)pLow);
    if (node == 0)
        return;

    do
    {
        unsigned int* nextLow  = (unsigned int*)((char*)node + 0x24);
        unsigned int* nextHigh = (unsigned int*)((char*)node + 0x28);

        void* next;
        if ((*nextHigh & *nextLow) == 0xFFFFFFFF)
            next = 0;
        else
            next = QueryHandleField0x54((SOptionalHandleRef*)nextLow);

        ((ChainNodeDispatchFn)(*(void***)node)[0x170 / 4])(node, arg0, arg4, arg8);

        node = next;
    } while (node != 0);
}
