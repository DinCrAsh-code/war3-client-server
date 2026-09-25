//============================================================================
//  0x6F49E850 - "is this mover still busy, and if it is not, tell it so".
//
//  Reached from 0x6F4A00B0 as the last thing that happens when a mover's
//  registration is torn down.  It answers 1 - still busy - the moment the
//  field at +0x38 is anything but -1, which is the same "no pending
//  request" sentinel CPathClient's own request slots use.  Otherwise, if
//  the low 24 bits of +0x3C are all clear, it dispatches the object's own
//  vtable slot 4 with a single zero argument and answers 0.
//
//  The object arrives as a *stack* argument, not in ecx, so this is a free
//  function over it rather than a member - the shipped code's own
//  `mov ecx, [esp+arg_0]` is the load, and ecx being the register it lands
//  in is what makes the slot-4 dispatch a plain __thiscall afterwards.
//
//  What the object is is not established here: nothing in this call tree
//  reaches its class, and the two fields it reads are the only ones
//  touched.  It is declared as its own opaque type for that reason rather
//  than guessed onto CPathClient, whose module it merely shares.
//============================================================================

namespace
{
    struct SPendingRequestOwner
    {
        char         m_reserved00[0x38];
        int          m_pending;      // +0x38 - -1 = nothing outstanding
        unsigned int m_flags;        // +0x3C - only the low 24 bits are read
    };

    //  vtable slot 4 (+0x10), one stack argument.  Its target is whatever
    //  the concrete class puts there; the argument count is read off this
    //  call site's single `push 0` and nothing else, which is why the
    //  typedef says one argument and no more.
    typedef void (__thiscall *NotifyIdleFn)(void* self, int reason);
}

int __stdcall CancelPendingRequestIfIdle(void* owner)
{
    SPendingRequestOwner* p = (SPendingRequestOwner*)owner;

    if (p->m_pending != -1)
        return 1;

    if ((p->m_flags & 0x00FFFFFF) == 0)
        ((NotifyIdleFn)(*(void***)p)[0x10 / 4])(p, 0);

    return 0;
}
