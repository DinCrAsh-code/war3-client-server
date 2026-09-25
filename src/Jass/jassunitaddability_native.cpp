//============================================================================
//  0x6F3C82A0 - JASS_UnitAddAbility, the native's own top-level entry -
//  the root of this closure (jassunitaddability_calltree_asm.md) and the
//  last thing this batch does, once every callee below is in place.
//
//  __cdecl, two stack arguments (unit handle, ability rawcode), bare
//  `retn` - the same shape JASS_UnitAddItemById already has
//  (jassunitadditembyid_native.cpp).
//
//  What it does: resolve the unit; refuse if it already has an ability of
//  this exact rawcode (SUnitAbilityHost::FindAbility, exact match only -
//  `a=0` disables the derived-type fallback); otherwise run the shared
//  ability-chain grant/notify sequence this closure shares with
//  JASS_CreateUnit's own G4/G5 ability-manager work
//  (Item/itemapplypowerupeffect.cpp, Item/itemabilitybatch2_thunks.cpp):
//  a pre-grant refresh (SRecipientRefresh::RefreshA), the grant itself
//  (GrantAbilityToRecipient), an unconditional post-grant refresh
//  (SRecipientRefresh::RefreshB), and - only on a successful grant - the
//  GameUI notify this batch reconstructs in full
//  (GameUI/gameuiabilitygrantnotify.cpp, GameUI/gameuiabilitygrantreceiver.cpp).
//  The return value is a plain
//  0/1 success flag, not a handle - unlike JASS_UnitAddItemById's item
//  token, this native's own JASS signature is `(unit, integer) -> boolean`.
//
//  The `eax != 0 -> 1 : 0` normalisation
//  (`neg esi` / `sbb esi,esi` / `neg esi`) is the shipped idiom for
//  "collapse any nonzero pointer to a bool", not something to fold into a
//  `!= 0` comparison - MSVC would emit a `setnz`/`test` shape for that
//  source instead, and the dump plainly has the sbb/neg pair.
//============================================================================
#include "unit.h"

//  0x6F3BDCB0 - the unit-handle resolver, already reconstructed
//  (Jass/jassresolveunithandle.cpp).
CUnit* __fastcall ResolveUnitHandle(int hUnit);

//  0x6F0787D0 - SUnitAbilityHost::FindAbility, already reconstructed for
//  real (Unit/unit_findability.cpp). Re-declared here on a minimal local
//  view of the receiver - only the method is reached, no field of it - the
//  same per-TU-local-declaration convention every other caller of this
//  address already uses (Unit/unit_flagbit8.cpp,
//  Unit/unit_abilityhostnotify.cpp, ...).
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  0x6F2B34D0 / 0x6F2B34A0 - SRecipientRefresh::RefreshA/RefreshB, already
//  reconstructed as naked redirects sharing this same class name
//  (Item/itemabilitybatch2_thunks.cpp) - re-declared, not redefined, to
//  avoid the two-symbols-for-one-address defect link_check.py catches.
struct SRecipientRefresh
{
    void __thiscall RefreshA();
    void __thiscall RefreshB();
};

//  0x6F24CDF0 - GrantAbilityToRecipient, already reconstructed as a naked
//  redirect (Item/itemabilitybatch2_thunks.cpp) - this closure's own giant
//  ability-type dispatcher, out of this batch's own worklist slice (its
//  own closure runs into several hundred further per-ability-type
//  handlers, not something one batch's slice was ever meant to reach).
//  __fastcall: recipient in ecx, packed rawcode in edx, three more stack
//  arguments - here all zero (no source item, no extra flags: a plain
//  rawcode grant).
void* __fastcall GrantAbilityToRecipient(void* recipient, unsigned int packedId,
                                          int arg4, int arg8, void* item);

//  0x6F332830 - this batch's own GameUI notify
//  (GameUI/unitabilitygrantnotifygameui.cpp).
void __fastcall NotifyGameUIAbilityGranted(void* recipient);

int __cdecl JASS_UnitAddAbility(int hUnit, int abilityId)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;

    //  Exact-rawcode-only lookup (a=0 disables the AgileTypeIsDerivedFrom
    //  fallback; b=0 compares the candidate's own reported code, not its
    //  +0x34 alternate; c=d=1 so both per-candidate gate bits let the
    //  check run): refuse a duplicate grant outright.
    if (((SUnitAbilityHost*)unit)->FindAbility(abilityId, 0, 1, 1, 1))
        return 0;

    ((SRecipientRefresh*)unit)->RefreshA();

    void* granted = GrantAbilityToRecipient(unit, abilityId, 0, 0, 0);

    //  Collapse the pointer to a 0/1 result - the shipped
    //  neg/sbb/neg idiom, not a `!= 0` MSVC would spell differently.
    int success = granted ? 1 : 0;

    ((SRecipientRefresh*)unit)->RefreshB();

    if (success)
        NotifyGameUIAbilityGranted(unit);

    return success;
}
