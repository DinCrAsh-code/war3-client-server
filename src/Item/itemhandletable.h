//============================================================================
//  A handle-index lookup table reached while resolving a JASS item handle
//  to its underlying agent object - see jasssetitemposition_native.cpp for
//  the caller chain and jassitemtypedassign.h for what happens to the
//  result.
//============================================================================
#ifndef ITEMHANDLETABLE_H
#define ITEMHANDLETABLE_H

#include "storm.h"

class CAgent;

//  sub_6F3A8060's own cached singleton object - a lazy Storm-style
//  bootstrap this session did not reconstruct (declared only, redirected
//  to its real address at 0x6F3A8060 - see jasssetitemposition_native.cpp)
//  because reaching further into it is exactly the "boring deep procedure"
//  this session's own instructions said to stay out of.  Only the one
//  field QueryItemSlotTable below actually reads is named.
//  0x6F428050 is a real member (this in ecx, index on the stack, `retn 4`)
//  - declared here rather than a free function because `__thiscall` on a
//  free function is rejected by this compiler (docs/msvc-vc8-idioms.md,
//  the same trap the ISetterTarget forwarders hit earlier this session).
struct SItemSlotTableHolder
{
    char                m_reserved00[0x94];
    //  +0x94 - the JASS "current event" context, one push-down stack of
    //  32-bit values per context slot.  The natives only ever look at the
    //  top of one: slot 4 holds the current CScriptEvent's handle, slot 5
    //  the current CScriptEventData's, and slot 6 the current event id
    //  (jassevents.h).  Seven is the highest index this batch reaches, not
    //  a length the shipped code states; whatever follows it is inside the
    //  reserved block below, whose start offset is what is exact.
    TSGrowableArray<int> m_eventSlots[7];   // +0x94 .. +0x103

    //  0x6F4280B0 - the top of one of those stacks, or 0 for an empty
    //  stack or a zero on top.  `retn 4`.
    int PeekEventSlot(int index);

    char                m_reserved104[0x19C - 0x104];
    unsigned char*      m_records;   // +0x19C - AGENT_SLOT-shaped, 12
                                      //          bytes each, index 0
                                      //          corresponds to handle
                                      //          index 0x100000

    //  0x6F428050 - see itemhandletable.cpp.
    CAgent* QueryItemSlot(int index);
};

//  0x6F2B5AE0 - the 'item' FourCC, spelled out in the constant's own bytes
//  (0x69,0x74,0x65,0x6D = "item"). A leaf exactly like sub_6F4266F0 below;
//  both are one-instruction FourCC getters this codebase's own naming
//  scheme for such constants (docs/msvc-vc8-idioms.md) has no better name
//  for than what byte pattern they return.
unsigned int __fastcall GetItemTypeFourCC();

//  0x6F4266F0 - a second FourCC getter, same one-instruction shape as
//  GetItemTypeFourCC above but a different constant (0x2B67616D) this call
//  tree gives no identifying use for on its own - only that
//  sub_6F3A8060 (the lazy singleton this repo did not dig into, per this
//  session's own instruction to stay out of "boring deep procedures")
//  feeds it to FourCC__Hash while building the table QueryItemSlotTable
//  reads below.
unsigned int __fastcall GetUnknownFourCC_6F4266F0();

#endif
