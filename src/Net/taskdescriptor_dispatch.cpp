//============================================================================
//  sub_6F1026D0 (0x6F1026D0) - part of the CItem::SetItemPosition/"ItemDrop"
//  live call chain (frame [3] of CItem__SetItemPosition_callchain.md,
//  reached from frame [4] sub_6F62A5D0 through *its* own vtable slot 3 on a
//  task-descriptor object, `this` in ecx, one stack argument, `retn 4`).
//
//  A genuine C++ switch over a task/event type id read from `ev->m_id`
//  (`[edx+8]`) - not a jump table (the ids are too sparse: 0x80261,
//  0xD0003, 0xD0022-0xD0027, 0xD0144, 0xD017D, 0xD017E, 0xD01A0, 0xD01A7,
//  0xD01AA, plus a default), so MSVC8 lowers it to the nested compare/
//  subtract bisection this reproduces. Every case falls through to the
//  same `return 1` - this function's whole job is "run the handler for
//  this event type", the return value is not otherwise used by its own
//  caller.
//
//  Two of these ids are read from a live capture, not IDA's static
//  analysis (the whole chain crosses several indirect vtable calls IDA
//  cannot resolve) - see asm/CItem_SetItemPosition_ItemDrop_chain.md's own
//  header.
//
//  **`0xD0144` does NOT call sub_6F0F9810**, which an earlier version of this
//  file asserted and named a function after.  The dump is explicit at
//  0x6F10273F: `sub eax, 0D0144h / jz loc_6F102754`, and 0x6F102754 is the
//  `this->field154` notify - so 0xD0144 takes the field154 path and
//  sub_6F0F9810 is the *default* for every other id in (0xD0027, 0xD017D).
//  Reconstructing it the other way round compiled, matched, hooked, ran, and
//  quietly broke item drops: the order was accepted and the item stayed in
//  the inventory.  The live capture that put sub_6F0F9810 in this call tree
//  was real; the inference that 0xD0144 was the id reaching it was not.
//  The rest of this switch's case handlers are not reconstructed - genuinely
//  separate trigger-action bodies, declared only so this dispatcher itself
//  compiles.
//============================================================================

struct TaskEventData
{
    char          pad[8];
    unsigned int  m_id;   // +8
};

struct TaskDescriptor
{
    char   pad[0x154];
    void*  m_field154;   // +0x154 - an object whose own method (slot?)
                          // gets called with argument 1 for every event id
                          // in the (0xD0027, 0xD017D) range except 0xD0144
                          // ("ItemDrop") itself; not otherwise identified.

    int Dispatch(TaskEventData* ev);
};

//  Every handler is __stdcall, taking its TaskEventData* on the stack.  The
//  dump shows `push edx` ahead of every one of these calls (hoisted above a
//  branch where two share it - 0x6F102702 and 0x6F102784), and a pushed
//  argument is one the callee cleans.  Ten of these were declared __fastcall,
//  which made MSVC emit `call @Handler_X@4` with nothing pushed; the real,
//  unhooked handler then cleaned four bytes that were never there, leaving
//  the stack short on every call.  Hooking this function alone crashed the
//  game at eip=0x6660c86 - see docs/notes/mix-config-78-crashes.md and the
//  matching entry in unreconstructed_thunks.cpp, which defines the thunks
//  and has to agree with these declarations or nothing links.
extern "C"
{
    void __stdcall  Handler_PawnGiveDropItemRange(TaskEventData*);   // sub_6F0F9450
    void __stdcall  Handler_0xD0003(TaskEventData*);                  // sub_6F0F10E0 - "PickupItemRange"
    void __stdcall  Handler_0x80261(TaskEventData*, int, int);               // sub_6F0F17A0
    void __stdcall  Handler_0xD0022_0xD0027(TaskEventData*);          // sub_6F0F1810
    //  NOT the 0xD0144 handler, despite what this was called before: the
    //  dump routes 0xD0144 to the field154 notify below and sends every
    //  *other* id in (0xD0027, 0xD017D) here.  See the dispatch body.
    void __stdcall  RangeDefaultHandler_0xD0028_0xD017C(TaskEventData*);  // sub_6F0F9810
    void __stdcall  Handler_0xD017D(TaskEventData*);                  // sub_6F0F9A20
    void __stdcall  Handler_HighRangeDefault(TaskEventData*);         // sub_6F0F93F0
    void __stdcall  Handler_0xD01A0(TaskEventData*);                  // sub_6F0DB210
    void __stdcall  Handler_0xD017E(TaskEventData*);                  // sub_6F0FD1B0 - "PickupItemRange"
    void __stdcall  Handler_0xD01A7(TaskEventData*);                  // sub_6F0DB240
    void __stdcall  Handler_AboveD01A7Default(TaskEventData*);        // sub_6F0F9B80
    void __stdcall  Handler_0xD01AA(TaskEventData*);                  // sub_6F149E70
}

struct Field154Owner
{
    void Notify(int value);   // sub_6F479160
};

int TaskDescriptor::Dispatch(TaskEventData* ev)
{
    unsigned int id = ev->m_id;

    if (id > 0xD017D)
    {
        if (id > 0xD01A7)
        {
            if (id == 0xD01AA)
                Handler_0xD01AA(ev);
            else
                Handler_AboveD01A7Default(ev);
        }
        else if (id == 0xD01A7)
            Handler_0xD01A7(ev);
        else if (id == 0xD017E)
            Handler_0xD017E(ev);
        else if (id == 0xD01A0)
            Handler_0xD01A0(ev);
        else
            Handler_HighRangeDefault(ev);
    }
    else if (id == 0xD017D)
    {
        Handler_0xD017D(ev);
    }
    else if (id > 0xD0027)
    {
        //  This way round, and it matters: the shipped code is
        //  `sub eax, 0D0144h / jz loc_6F102754` at 0x6F10273F, so it is
        //  *0xD0144* that takes the field154 path and every other id in
        //  (0xD0027, 0xD017D) that calls sub_6F0F9810.  This was inverted,
        //  and the user found it live: a unit could be ordered to drop an
        //  item, the order ran, and the item stayed in the inventory -
        //  because the drop id was being routed to the range's default
        //  handler instead of to the field154 notify that performs it.
        if (id != 0xD0144)
            RangeDefaultHandler_0xD0028_0xD017C(ev);
        else if (m_field154)
            ((Field154Owner*)m_field154)->Notify(1);
    }
    else if (id >= 0xD0022)
    {
        Handler_0xD0022_0xD0027(ev);
    }
    else if (id == 0x80261)
    {
        Handler_0x80261(ev, -1, -1);
    }
    else if (id == 0xD0003)
    {
        Handler_0xD0003(ev);
    }
    else
    {
        Handler_PawnGiveDropItemRange(ev);
    }

    return 1;
}
