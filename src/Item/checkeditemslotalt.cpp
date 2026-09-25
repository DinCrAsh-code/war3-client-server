//============================================================================
//  0x6F273480 - a type-checked, reference-counted CAgent* Assign, the same
//  "AssignChecked" shape agent.h's own CAgentPtr and the whole
//  Agent/agenttypedslots.h family share (see allytypedassign.cpp for the
//  clearest sibling): check the candidate's own GetAgileTypeId() against a
//  fixed required tag, swap the held pointer only if the checked candidate
//  differs from what is already held, release the old one, add-ref the new
//  one.
//
//  Not one of the eighteen Agent/agenttypedslots.h members - this address is
//  outside that family's own list, and the required tag (GetTypeTag_6F2711B0,
//  already EXACT, a bare `mov eax, 75697465h`) is not any of the FourCCs
//  that header's table already knows.  Reached from this batch's own
//  0x6F2994D0 and 0x6F27FB20 (the latter's converting constructor,
//  Item/checkeditemslotalt_ctor.cpp) plus three further CItem-notify
//  addresses this call tree does not define the bodies of
//  (0x6F293990/0x6F299750, and SRecipientItemNotify::NotifyItem's own
//  redirect at 0x6F299610) - so this is CItem's own family, kept in its own
//  small struct rather than folded into the Agent-wide table.
//
//  Own translation unit: two real calls out of it (GetAgileTypeId is a
//  virtual slot, not a call this repo names by address).
//============================================================================
#include "agent.h"
#include "agiletype.h"

//  0x6F2711B0 - already EXACT (Misc/trivialaccessors_02_03.cpp).  Ignores
//  its one argument; called here with `this` still sitting in ecx from this
//  function's own entry, unchanged since the caller never touches it before
//  the call (the same "callee relies on the caller having left it there"
//  idiom docs/msvc-vc8-idioms.md already documents).
unsigned int __fastcall GetTypeTag_6F2711B0(void* self);

struct SCheckedItemPtrSlotAlt
{
    CAgent* m_value;      // +0x00

    //  0x6F27FB20 - the converting constructor: zero m_value, then
    //  AssignChecked(candidate).  The shipped body wraps this in a
    //  cookie-XORed __except_handler4 frame (`push offset SEH_6F27FB20` /
    //  `mov large fs:0`) this reconstruction does not reproduce at its
    //  fixed /GS- /EHs-c- - see docs/msvc-vc8-idioms.md, "An
    //  `__except_handler4`-shaped frame this toolchain cannot reproduce".
    //  The body either side of that frame is the same sequence.
    SCheckedItemPtrSlotAlt(CAgent* candidate);

    SCheckedItemPtrSlotAlt* AssignChecked(CAgent* candidate);
};

SCheckedItemPtrSlotAlt::SCheckedItemPtrSlotAlt(CAgent* candidate)
{
    m_value = 0;
    AssignChecked(candidate);
}

SCheckedItemPtrSlotAlt* __thiscall
SCheckedItemPtrSlotAlt::AssignChecked(CAgent* candidate)
{
    CAgent* checked;
    if (candidate &&
        AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetTypeTag_6F2711B0(this)))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}
