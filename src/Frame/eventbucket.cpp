//============================================================================
//  0x6F631DC0 - CEventBucketList::InsertHandler.
//
//  One EVTHANDLER record (".?AUEVTHANDLER@@", 0x18 bytes) per binding,
//  spliced into slot `slot`'s own list at the point that keeps it ordered by
//  `when`: the walk runs backwards from the tail and stops at the last entry
//  whose own time is not greater than the new one *and* whose +0x14 is clear,
//  and the new record goes immediately after it - or at the head when there
//  is no such entry.
//
//  A null receiver is Storm error 0x57 and nothing else, which is why the
//  null test has its own `retn 0Ch` ahead of the frame the rest needs.
//
//  The list's link offset is a run-time field here (the shipped walk reloads
//  it from +0x5C on every hop), so this is TSExplicitList and not TSList.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void __fastcall CEventBucketList::InsertHandler(int slot, void* handler,
                                                int a, float when)
{
    if (this == 0)
    {
        SErrSetLastError(0x57);
        return;
    }

    //  The bucket's address is formed before the allocation, which is what
    //  the shipped code does (`lea eax,[edx+edx*2]` / `lea ebx,[ecx+eax*4+5Ch]`
    //  ahead of the SMemAlloc call, held in ebx across it).  Naming it after
    //  the call makes MSVC re-form it from `slot` afterwards instead.
    SEventHandlerList& list = m_slots[slot];

    SEventHandler* node = new ((SEventHandler*)SMemAlloc(
        0x18, ".?AUEVTHANDLER@@", -2, 8)) SEventHandler;

    node->m_when = when;
    node->m_a = a;
    node->m_pHandler = handler;

    int link = list.TailLink();
    SEventHandler* after = 0;
    while (link > 0)
    {
        SEventHandler* candidate = (SEventHandler*)link;
        //  `!(x > when)`, not `x <= when`: the shipped test is
        //  `test ah,41h` / `je`, which takes the unordered case with
        //  the not-greater one.  Spelling it `<=` gives `jp` instead,
        //  which is a different answer for a NaN time.
        if (!(candidate->m_when > when) && candidate->m_14 == 0)
        {
            after = candidate;
            break;
        }
        link = (int)list.Prev(candidate);
    }

    list.LinkAfter(after, node);
}
