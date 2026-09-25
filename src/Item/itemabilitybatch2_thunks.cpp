//============================================================================
//  Pass-through definitions for the callees CAbilityInventory::
//  StoreItemIntoSlot (0x6F0F8C10, abilitystoreslot.cpp) and
//  SItemPowerupSource::ApplyPowerupEffect (0x6F2B9600,
//  itemapplypowerupeffect.cpp) declare and call but do not reconstruct -
//  the same contract jassnatives_thunks.cpp's own header explains: without
//  these the tree does not link, each body jumps to the shipped function at
//  its real address, and none of them is a reconstruction or ever offered
//  as hookable.
//
//  Every declaration's argument bytes come from `tools/ida_query.py
//  func_at`/`bytes` on the address itself (CLAUDE.md's own recipe for the
//  "no dump body" case) - none of these ten addresses has a dump
//  anywhere in asm/, since IDA's own call-tree walk stopped one level short
//  of them.  What each one *does* is not established beyond what its own
//  call site's argument shapes already show; the receiver types below are
//  local, single-method stand-ins the same way SAttachmentOwnerQuery
//  already is in abilityapplypowerup.cpp, not claims about a real class.
//============================================================================
#include "unitinventory.h"

#define BATCH2_THUNK(addr)      \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//----------------------------------------------------------------------------
//  StoreItemIntoSlot's own five.  0x6F285740, 0x6F2B8D70, 0x6F3329B0 and
//  0x6F423D40 used to be here too - all four turned out to already be real
//  reconstructions from the concurrent CUnit inventory-closure batch
//  (CUnit::WantsSubgroupRefreshNotify, ItemTypeAbilitySlotCategory,
//  CUnit::NotifySubgroupChanged, SPlayerSubgroupTarget::
//  NotifyAsyncRefreshSubgroups), used directly by abilitystoreslot.cpp now
//  rather than defined a second time under a new name for the same
//  address.
//----------------------------------------------------------------------------

//  0x6F2B7EF0 / 0x6F2B7F40 - `retn 4` apiece, called on the item with one
//  out-pointer each (the values StoreItemIntoSlot's own price-comparison
//  block feeds through `fld`).
struct SItemPriceFieldQuery
{
    void __thiscall QueryFieldA(void* out);
    void __thiscall QueryFieldB(void* out);
};
__declspec(naked) void SItemPriceFieldQuery::QueryFieldA(void*) BATCH2_THUNK(0x6F2B7EF0)
__declspec(naked) void SItemPriceFieldQuery::QueryFieldB(void*) BATCH2_THUNK(0x6F2B7F40)

//  0x6F2B7040 - `retn 8`, called on the item with two out-pointers, only
//  when the price comparison above picks the second candidate.
struct SItemPriceFieldWriter
{
    void __thiscall WriteFields(void* out1, void* out2);
};
__declspec(naked) void SItemPriceFieldWriter::WriteFields(void*, void*) BATCH2_THUNK(0x6F2B7040)

//  0x6F26FE20 - `retn 0`.  A tail jump to 0x6F332810 in the shipped image
//  (confirmed by func_at); declared at its own address since that is what
//  StoreItemIntoSlot's own call site names.
struct SRecipientNotifyA
{
    void __thiscall NotifyA();
};
__declspec(naked) void SRecipientNotifyA::NotifyA() BATCH2_THUNK(0x6F26FE20)

//  0x6F299610 - `retn 4`, called on the recipient with the item pointer.
struct SRecipientItemNotify
{
    void __thiscall NotifyItem(void* item);
};
__declspec(naked) void SRecipientItemNotify::NotifyItem(void*) BATCH2_THUNK(0x6F299610)

//----------------------------------------------------------------------------
//  SItemPowerupSource::ApplyPowerupEffect's own five.  0x6F079CC0 and
//  0x6F3A35C0 used to be here too - SUnitAbilityHost::RemoveAbility
//  (Unit/unit_flagbit8thunks.cpp) and CGameWorld::TestDetectionRelationBit
//  (GameUI/gameui.h) respectively, both already real reconstructions from
//  the concurrent CUnit closure batch; declared locally in
//  itemapplypowerupeffect.cpp under their real names rather than defined a
//  second time here.
//----------------------------------------------------------------------------

//  0x6F2B34D0 / 0x6F2B34A0 - `retn 0` apiece, both called on the recipient
//  with no arguments, at two different points in the ability-list walk.
struct SRecipientRefresh
{
    void __thiscall RefreshA();
    void __thiscall RefreshB();
};
__declspec(naked) void SRecipientRefresh::RefreshA() BATCH2_THUNK(0x6F2B34D0)
__declspec(naked) void SRecipientRefresh::RefreshB() BATCH2_THUNK(0x6F2B34A0)

//  0x6F24CDF0 - `retn 0Ch` (three stack dwords) plus two register
//  arguments - a genuine `__fastcall` shape, not a `this`-bearing member:
//  the shipped call site's `ecx`/`edx` pair is the recipient and a packed
//  fourCC id, both register-only, with three more values on the stack.
void* __fastcall GrantAbilityToRecipient(void* recipient, unsigned int packedId,
                                          int arg4, int arg8, void* item)
                                                                    BATCH2_THUNK(0x6F24CDF0)

//  0x6F3333D0 - `retn 4`, `__fastcall`-shaped (ecx/edx both loaded with
//  plain ints at the call site, not a receiver) plus one stack argument.
void __fastcall LogAbilityListWarning(int a, int b, int c) BATCH2_THUNK(0x6F3333D0)

//  0x6F2B6160 - `retn 0`, called on the item itself with no arguments, at
//  the very end of ApplyPowerupEffect.
struct SItemFinalize
{
    void __thiscall Finalize();
};
__declspec(naked) void SItemFinalize::Finalize() BATCH2_THUNK(0x6F2B6160)

#undef BATCH2_THUNK
