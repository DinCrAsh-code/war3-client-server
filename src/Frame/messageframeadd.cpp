//============================================================================
//  0x6F623830 - CMessageFrame::AddMessage.
//
//  Three splices and one allocation each.  The line itself goes on the tail
//  of the live list at +0x178; then it joins the *display* list at +0x184,
//  either at the tail (when the caller does not ask for a position) or
//  immediately after the last entry still carrying the "pending" bit, which
//  is cleared as it is passed; and finally, when the caller gave it a
//  positive lifetime, an expiry record goes on the head of the timer list at
//  +0x1A8.
//
//  Both tail splices are TSExplicitList::LinkToTail written out - the
//  shipped code has them inlined and they are the only two call sites of
//  it in this repo, which is why that member is `__forceinline`.  The link
//  the splice operates on is `Link(node)`, and `Link(0)` being the
//  terminator is what makes a failed allocation take the same code path
//  instead of a second one: the whole "the node is null" case is one
//  `lea ecx, [esi+17Ch]`.
//
//  The lifetime test is `0.0f < timer` and not `timer != 0`: the shipped
//  `fldz` / `fcomp` / `test ah,1` is C0 alone, the strictly-less mask
//  (docs/msvc-vc8-idioms.md's condition-code table).
//============================================================================
#include "messageframe.h"

void CMessageFrame::AddMessage(const char* text, float timer, int field04,
                               const int* colour, int insertByPriority)
{
    TEXTMSG* message = (TEXTMSG*)SMemAlloc(sizeof(TEXTMSG),
                                           ".?AUTEXTMSG@@", -2, 8);
    if (message != 0)
    {
        message->m_text = 0;
        message->m_field04 = 0;
        message->m_colour = 0;
        message->m_field0C = 0;
        *(int*)&message->m_reserved18[0x18 - 0x18] = 0;
        *(int*)&message->m_reserved18[0x1C - 0x18] = 0;
        *(int*)&message->m_reserved18[0x20 - 0x18] = 0;
        *(int*)&message->m_reserved18[0x24 - 0x18] = 0;
        *(int*)&message->m_reserved18[0x28 - 0x18] = 0;
        *(int*)&message->m_reserved18[0x2C - 0x18] = 0;
        *(int*)&message->m_reserved18[0x30 - 0x18] = 0;
        *(int*)&message->m_reserved18[0x34 - 0x18] = 0;
        message->m_flags = 0;
    }

    m_messages.LinkToTail(message);

    message->m_text = Storm_507(text, ".\\CMessageFrame.cpp", 0x142);
    message->m_field04 = field04;
    message->m_colour = *colour;
    message->m_timer = timer;

    unsigned int flags = (message->m_flags & ~1u) | 2u;

    if (insertByPriority != 0)
    {
        message->m_flags = flags;

        //  Walk the display list back from its tail looking for the last
        //  entry that still has the pending bit; that bit is cleared as the
        //  new line is spliced in after it.  Falling off the end splices at
        //  the head, which is what `after == 0` means to InsertAfter.
        TEXTMSG* after = (TEXTMSG*)m_display.TailLink();
        if ((int)after > 0)
        {
            for (;;)
            {
                if ((after->m_flags & 2) != 0)
                {
                    after->m_flags &= ~2u;
                    InsertAfter(message, 1, after);
                    return;
                }

                after = m_display.Prev(after);
                if ((int)after <= 0)
                    break;
            }
        }

        InsertAfter(message, 1, 0);
        return;
    }

    message->m_flags = flags;
    m_display.LinkToTail(message);

    if (0.0f < timer)
    {
        TIMERDATA* expiry = (TIMERDATA*)SMemAlloc(sizeof(TIMERDATA),
                                                  ".?AUTIMERDATA@@", -2, 8);
        if (expiry != 0)
        {
            expiry->m_link.m_next = 0;
            expiry->m_link.m_prevlink = 0;
        }

        m_timers.LinkToHead(expiry);

        expiry->m_when = timer;
        expiry->m_owner = &m_reserved190[0];
        expiry->m_message = message;
        expiry->m_field0C = 0;
    }
}
