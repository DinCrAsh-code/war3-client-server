//============================================================================
//  0x6F6329E0 - RemoveEventHandlers: drop bindings out of every one of a
//  bucket list's 0x1D handler slots.
//
//  Three filters, and `flags` says which of them are live: bit 0 restricts
//  the sweep to the single slot `slot`, bit 1 makes the handler pointer have
//  to match, bit 2 makes the `a` value have to match.  A record whose +0x14
//  is set is never removed whatever the filters say - that is the "in use
//  right now" mark the dispatcher leaves behind.  Every caller in this
//  closure passes -1, i.e. all three on.
//
//  A free `__fastcall` and not a member of CEventBucketList: the shipped
//  body takes the list in ecx *and* the slot index in edx, and no MSVC
//  member spelling puts a second argument in edx
//  (docs/msvc-vc8-idioms.md, "A receiver in ecx with a second argument in
//  edx is a free `__fastcall`").  A null list is Storm error 0x57 and its
//  own `retn 0Ch` ahead of the frame the rest needs, exactly as
//  CEventBucketList::InsertHandler has it.
//
//  Its own translation unit: UnregisterEventHandler calls it for real.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  The predecessor, masked down to "a real node or null".  The shipped code
//  masks it on the path that is about to unlink the node - because the value
//  becomes the walk's next cursor after the record it came from is gone -
//  and leaves it raw on the path that just steps.  Two accessors, one per
//  path, is what storm.h's own Tail()/TailLink() pair already is.
static SEventHandler* MaskedPrev(SEventHandlerList& list, SEventHandler* node)
{
    int prevlink = list.Link(node)->m_prevlink;
    return prevlink > 0 ? (SEventHandler*)prevlink : 0;
}

void __fastcall RemoveEventHandlers(CEventBucketList* list, int slot,
                                    void* handler, int a, int flags)
{
    if (list == 0)
    {
        SErrSetLastError(0x57);
        return;
    }

    int oneSlotOnly = flags & 1;

    for (int index = 0; index < 0x1D; index++)
    {
        if (oneSlotOnly == 0 || index == slot)
        {
            SEventHandlerList& bucket = list->m_slots[index];

            int link = (int)bucket.Tail();
            while (link > 0)
            {
                SEventHandler* node = (SEventHandler*)link;

                if ((node->m_pHandler == handler || (flags & 2) == 0) &&
                    (node->m_a == a || (flags & 4) == 0) &&
                    node->m_14 == 0)
                {
                    SEventHandler* prev = MaskedPrev(bucket, node);
                    node->m_link.Unlink();
                    SMemFree(node, ".?AUEVTHANDLER@@", -2, 0);
                    link = (int)prev;
                }
                else
                {
                    link = (int)bucket.Prev(node);
                }
            }
        }
    }
}
