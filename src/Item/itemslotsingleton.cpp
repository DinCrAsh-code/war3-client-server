//============================================================================
//  0x6F3A8060 - CItemSlotSingletonHost::GetSlotTable: the lazy singleton
//  every JASS item-handle resolution goes through (itemhandlemain.cpp).
//
//  Was a `mov eax,<address> / jmp eax` redirect, described as "a lazy
//  Storm-style singleton bootstrap ... this session did not reconstruct".
//  It is thirty-eight instructions, and its whole body is a shape this repo
//  already has twice: CWidget::ClearPlayerMaskBits (widget_slot68.cpp)
//  builds the same 0x2C-byte request block, from the same registry lookup,
//  and hands it to the same maker.
//
//  Reconstructing it is not cosmetic.  As a thunk it is a **self-redirect**
//  - patching its own address to point at a body whose first act is to jump
//  back to that address - which docs/notes/hook-abi-crash-classes.md
//  records as a live hang, confirmed by the operator.  A real body removes
//  the whole failure mode.
//
//  The lazy check, the fill and the answer all go through `this+0x1C`, and
//  the shipped code takes its *address* once (`lea esi,[ecx+1Ch]`) and uses
//  that for the slot assign, the follow-up call and the return - so the
//  field is a slot object, not a bare pointer, and the assign is a member
//  of it.
//
//  The explicit -1 into the block's +0x24 stays: InitWidgetAgentQuery fills
//  the block, and this caller then overwrites that one word before
//  submitting.  Both stores are in the shipped code.
//
//  Own translation unit: ResolveItemHandle and two more callers reach this
//  with real calls, and it makes seven of its own.
//============================================================================
#include "itemhandlemain.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"
#include "agenttypedslots.h"
#include "agentregistrysingleton.h"

//----------------------------------------------------------------------------
//  0x6F39F800 - park the made agent in the slot.  Was a naked `jmp`
//  thunk described as "ninety-odd instructions of registry bookkeeping,
//  out of scope"; it is forty-two, and it is the *fourth* copy of
//  SCheckedItemSlot::Assign - the type-checked agent slot - against
//  '+gam' rather than 'item'.  See agenttypedslots.h: the whole family
//  is one shape, and the item copy at 0x6F02A6A0 scores 1.000 EXACT.
//
//  `SCheckedGameSlot` is that type; the slot at this+0x1C *is* one, which
//  is why the shipped code takes its address once and reuses it for the
//  assign, the finish call and the return.
//----------------------------------------------------------------------------
SCheckedGameSlot* SCheckedGameSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetUnknownFourCC_6F4266F0()))
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

//----------------------------------------------------------------------------
//  0x6F42C210 - the holder's own "you are the singleton now" finish step.
//  Reconstructed in agentregistrysingleton.cpp (its own module); declared
//  here, called here.  `retn` with nothing on the stack - a thiscall
//  member with no arguments.
//----------------------------------------------------------------------------

//  The maker hands back a holder whose +0x54 is the object this wants - the
//  same `mov eax,[eax+54h]` widget_slot68.cpp reads through SMadeAgentHolder.
struct SMadeSlotTableHolder
{
    char                  m_reserved00[0x54];
    SItemSlotTableHolder* m_pTable;     // +0x54
};

SItemSlotTableHolder* CItemSlotSingletonHost::GetSlotTable()
{
    SCheckedGameSlot* slot = (SCheckedGameSlot*)((char*)this + 0x1C);

    if (slot->m_value == 0)
    {
        //  The singleton held in a callee-saved register across Hash(), the
        //  same way CAgent::ReleaseSelf and CWidget::ClearPlayerMaskBits do
        //  it - written after the Hash() call it is reloaded instead.
        AGILE_TYPE_ID id(GetUnknownFourCC_6F4266F0());
        CGameData* game = g_pGameData;
        unsigned int hash = id.Hash();
        AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

        SWidgetAgentQuery query;
        InitWidgetAgentQuery(&query, GetUnknownFourCC_6F4266F0(),
                             type->m_allocator);
        query.m_reserved24 = 0xFFFFFFFF;

        slot->Assign((CAgent*)((SMadeSlotTableHolder*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pTable);

        ((CAgentRegistrySingleton*)slot->m_value)->FinishSingleton();
    }

    return (SItemSlotTableHolder*)slot->m_value;
}
