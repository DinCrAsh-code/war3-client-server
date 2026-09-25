//============================================================================
//  0x6F077FA0 - SUnitValueSource::Accumulate.
//
//  Zero the caller's two blocks, then walk the chain of contributors that
//  hangs off the unit's own {handle, typeTag} pair at +0x1DC, asking each
//  one for its own pair of blocks (vtable slot 0xB5) and adding them in.
//
//  The chain is threaded through handles, not pointers: each node carries
//  the next node's pair at its own +0x24, with the same all-ones "not set"
//  sentinel tested as `(typeTag & handle) == -1` that
//  Item/itemhandleresolve.h already records, and the same resolver behind
//  it.  The *next* node is resolved at the top of each iteration, before
//  the current one is asked for anything, which is what lets the loop end
//  on a plain "was there a next" test at the bottom.
//
//  Both the zeroing and the accumulate run the same two-element loop over
//  the block's first three groups - it is unrolled on both sides, and the
//  a/b interleave in the shipped store order is what says it is one loop
//  over both blocks rather than two.
//
//  The five `- esi` deltas the shipped prologue computes are MSVC's own
//  induction-variable elimination over that loop, not something the source
//  asks for.
//============================================================================
#include "unitvalueblock.h"
#include "itemhandleresolve.h"

typedef void (__thiscall *ValueContributorFn)(void* self, SUnitValueBlock* a,
                                              SUnitValueBlock* b);

struct SUnitValueChainHead
{
    char               m_reserved000[0x1DC];
    SOptionalHandleRef m_next;    // +0x1DC
};

void SUnitValueSource::Accumulate(SUnitValueBlock* a, SUnitValueBlock* b)
{
    if (a == 0)
        return;
    if (b == 0)
        return;

    for (unsigned int k = 0; k < 2; k++)
    {
        (&a->m_count04)[k] = 0;
        (&a->m_f10)[k] = g_CFloatZero;
        (&a->m_f1C)[k] = g_CFloatZero;
        (&b->m_count04)[k] = 0;
        (&b->m_f10)[k] = g_CFloatZero;
        (&b->m_f1C)[k] = g_CFloatZero;
    }

    a->m_f24 = g_CFloatZero;
    a->m_f28 = g_CFloatZero;
    a->m_total50 = 0;
    a->m_total54 = 0;
    a->m_total58 = 0;
    a->m_f2C = g_CFloatZero;
    a->m_f30 = g_CFloatZero;
    a->m_f34 = g_CFloatZero;
    a->m_f38 = g_CFloatZero;
    a->m_f40 = g_CFloatZero;
    a->m_f3C = g_CFloatZero;
    a->m_f44 = g_CFloatZero;
    a->m_f48 = g_CFloatZero;
    a->m_f4C = g_CFloatZero;

    b->m_f24 = g_CFloatZero;
    b->m_f28 = g_CFloatZero;
    b->m_total50 = 0;
    b->m_total54 = 0;
    b->m_total58 = 0;
    b->m_f2C = g_CFloatZero;
    b->m_f30 = g_CFloatZero;
    b->m_f34 = g_CFloatZero;
    b->m_f38 = g_CFloatZero;
    b->m_f40 = g_CFloatZero;
    b->m_f3C = g_CFloatZero;
    b->m_f44 = g_CFloatZero;
    b->m_f48 = g_CFloatZero;
    b->m_f4C = g_CFloatZero;

    SOptionalHandleRef* head = &((SUnitValueChainHead*)this)->m_next;
    if ((head->m_typeTag & (int)head->m_handle) == -1)
        return;

    void* node = QueryHandleField0x54(head);
    if (node == 0)
        return;

    for (;;)
    {
        SOptionalHandleRef* link = (SOptionalHandleRef*)((char*)node + 0x24);

        void* next;
        if ((link->m_typeTag & (int)link->m_handle) == -1)
            next = 0;
        else
            next = QueryHandleField0x54(link);

        SUnitValueBlock first(kCFloatNoInit);
        SUnitValueBlock second(kCFloatNoInit);
        first.Init();
        second.Init();

        ((ValueContributorFn)(*(void***)node)[0x2D4 / 4])(node, &first,
                                                          &second);

        for (unsigned int k = 0; k < 2; k++)
        {
            (&a->m_count04)[k] += (&first.m_count04)[k];
            (&a->m_f10)[k] = (&a->m_f10)[k] + (&first.m_f10)[k];
            (&a->m_f1C)[k] = (&a->m_f1C)[k] + (&first.m_f1C)[k];
            (&b->m_count04)[k] += (&second.m_count04)[k];
            (&b->m_f10)[k] = (&b->m_f10)[k] + (&second.m_f10)[k];
            (&b->m_f1C)[k] = (&b->m_f1C)[k] + (&second.m_f1C)[k];
        }

        a->m_f24 = a->m_f24 + first.m_f24;
        a->m_f28 = a->m_f28 + first.m_f28;
        a->m_total50 += first.m_total50;
        a->m_total54 += first.m_total54;
        a->m_total58 += first.m_total58;
        a->m_f2C = a->m_f2C + first.m_f2C;
        a->m_f30 = a->m_f30 + first.m_f30;
        a->m_f34 = a->m_f34 + first.m_f34;
        a->m_f38 = a->m_f38 + first.m_f38;
        a->m_f40 = a->m_f40 + first.m_f40;
        a->m_f3C = a->m_f3C + first.m_f3C;
        a->m_f44 = a->m_f44 + first.m_f44;
        a->m_f48 = a->m_f48 + first.m_f48;
        a->m_f4C = a->m_f4C + first.m_f4C;

        b->m_f24 = b->m_f24 + second.m_f24;
        b->m_f28 = b->m_f28 + second.m_f28;
        b->m_total50 += second.m_total50;
        b->m_total54 += second.m_total54;
        b->m_total58 += second.m_total58;
        b->m_f2C = b->m_f2C + second.m_f2C;
        b->m_f30 = b->m_f30 + second.m_f30;
        b->m_f34 = b->m_f34 + second.m_f34;
        b->m_f38 = b->m_f38 + second.m_f38;
        b->m_f40 = b->m_f40 + second.m_f40;
        b->m_f3C = b->m_f3C + second.m_f3C;
        b->m_f44 = b->m_f44 + second.m_f44;
        b->m_f48 = b->m_f48 + second.m_f48;
        b->m_f4C = b->m_f4C + second.m_f4C;

        if (next == 0)
            break;

        node = next;
    }
}
