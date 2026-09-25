//============================================================================
//  0x6F4BE9E0 - drop one node out of the prefetch cache: give its bytes back
//  to the byte count, take it out of both of the table's lists and hand it
//  to the table's own node allocator through vtable slot 0.
//
//  **The shipped build takes its argument in `esi`.**  Nothing writes esi
//  before the first read (`mov eax,[esi+1Ch]`), and both call sites leave the
//  node there - 0x6F4BEA10 puts the masked list tail in esi and
//  0x6F4C1550 still has the node in esi from its own earlier work.  That is
//  the whole-program register assignment docs/msvc-vc8-idioms.md records
//  under "Arguments in eax and esi"; no MSVC calling convention reaches esi,
//  so this takes the node in ecx and every caller pays one `mov`.
//============================================================================
#include "filecache.h"

void __fastcall DropPrefetchNode(PrefetchNode* node)
{
    unsigned int size = node->m_size;
    if (size == 0)
        return;

    g_prefetchBytes -= size;

    g_prefetchTable.Unlink(node);

    //  Slot 0 of the table's own vtable: give the node back.  The table is
    //  a plain object with no `virtual` of ours, so the slot is spelled as
    //  the dispatch the shipped code has - see Containers/allochashtable.h.
    typedef void (__thiscall *FreeNodeFn)(void*, PrefetchNode*);
    ((FreeNodeFn)(*(void***)&g_prefetchTable)[0])(&g_prefetchTable, node);

    g_prefetchCount -= 1;
}
