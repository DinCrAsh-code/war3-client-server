//============================================================================
//  0x6F2DE140 - CUnit::DrainAbilityGrants.  `this` in ecx, `retn 0`.
//
//  Resolve the unit's own owner (+0x30, or SAttachmentOwnerQuery::QueryOwner
//  the slow way when unset - the same fallback ResolveAndNotifyPosition and
//  GrantAbilityBySlot both already use), read that owner's own owning
//  player index (CUnit::GetOwningPlayerIndex, vtable slot 59) and look the
//  player up through IndexedArrayHolder_6F3A1650::At (already reconstructed,
//  unit_abilityhostnotify.cpp's own re-declaration) the same way
//  CUnit::NotifyAbilityHostChanged does; resolve *that* player's own
//  SPlayerScore::m_ref2D4 handle-ref pair (same cross-link) to reach the
//  owning SAbilityHostRecord.
//
//  If that record's own m_field40 is set, drain the unit's own +0x90
//  pending-grant counter: while it is nonzero, notify the record through
//  NotifyResolvedAt0x254(ownerRaw) and grant through
//  CUnitAbilityGrantSearch::GrantAbilityBySlot(that notify's own result, 0)
//  again and again, stopping as soon as +0x90 stops changing between
//  iterations (not just when it reaches zero) - the same "did this call
//  itself mutate the state" guard 0x6F2DBB90's own dump uses elsewhere in
//  this closure.
//============================================================================
#include "abilityhostrecord.h"
#include "unit.h"
#include "playerscore.h"

//  Re-declared to match Unit/unit_abilityhostnotify.cpp exactly, so the
//  call below binds to that definition rather than inventing a second
//  symbol (tools/link_check.py's first defect class).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  dword_6FAB65F4 - the world-object singleton (Unit/unit_publishposition.cpp
//  and friends).  Declared, never defined: the real game's global.
extern void* g_unk6FAB65F4;

//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, already reconstructed
//  (Widget/attachmentownerquery.cpp); re-declared here so the mangled name
//  binds to that one real symbol.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//  0x6F2095D0 - CUnitAbilityGrantSearch::GrantAbilityBySlot, already
//  reconstructed (abilityhostrecord_grantabilitybyslot.cpp).
struct CUnitAbilityGrantSearch
{
    void GrantAbilityBySlot(void* arg0, int forceAll);
};

//  `this` (a CUnit) in ecx, `retn 0` - a member, declared on a minimal
//  local receiver rather than on the shared CUnit itself (unit.h), the
//  same treatment CUnitAbilityGrantSearch already gets.
struct CUnitAbilityGrantDrain
{
    void DrainAbilityGrants();
};

void CUnitAbilityGrantDrain::DrainAbilityGrants()
{
    void* owner = *(void**)((char*)this + 0x30);
    if (owner == 0)
        owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    int playerIndex = ((CUnit*)owner)->GetOwningPlayerIndex();
    SPlayerScore* player = (SPlayerScore*)
        ((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(playerIndex);

    SAbilityHostRecord* ownerRecord = 0;
    if ((*(int*)((char*)player + 0x2D8) & *(int*)((char*)player + 0x2D4)) != -1)
        ownerRecord = (SAbilityHostRecord*)
            QueryHandleField0x54((SOptionalHandleRef*)((char*)player + 0x2D4));

    if (ownerRecord->m_field40 == 0)
        return;

    int* counter90 = (int*)((char*)this + 0x90);
    if (*counter90 == 0)
        return;

    int before;
    do
    {
        before = *counter90;
        void* resolved = ownerRecord->NotifyResolvedAt0x254(owner);
        ((CUnitAbilityGrantSearch*)this)->GrantAbilityBySlot(resolved, 0);
    } while (before != *counter90 && *counter90 != 0);
}
