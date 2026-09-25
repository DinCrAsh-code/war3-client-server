//============================================================================
//  0x6F079CA0 - CUnit::NotifyAbilityManagerAndGameUI.  `this` in ecx, one
//  stack pointer (the ability's own "manager"/template object), `retn 4`.
//  Reached from dozens of per-ability grant/revoke leaves and, by name,
//  from SUnitAbilityHost::GrantFlag_6F29EE30
//  (Unit/unit_abilitygrantflag.cpp)'s own NotifySingleton_6F079CA0 call.
//
//  Tell the unit's own ability host about the manager.  0x6F079990 already
//  carries a funcmap name from Unit/cunit_agent1_applyabilityeffect.cpp
//  (SUnitAbilityInstallLeaf::InstallAbilityAgent) - re-declared here
//  verbatim (same class, same member) so this call binds onto that
//  established name instead of a second, differently-named thunk symbol
//  for the same address (which compiles to a byte-identical `call`, but
//  a different displayed name than the dump's own resolved one).  Then
//  ask the manager itself to notify the game UI
//  (NotifyGameUIOfAbilityOwner, 0x6F0D1B90,
//  Unit/abilityownernotifygameui.cpp) - its own translation unit, a real
//  address 0x57F0 bytes away in a different original module, so the call
//  has to stay a real call rather than something /Ob2 folds in.
//============================================================================
#include "unit.h"

//  Re-declared to match Unit/cunit_agent1_applyabilityeffect.cpp exactly.
struct SUnitAbilityInstallLeaf { void __thiscall InstallAbilityAgent(void* agent); };

//  0x6F0D1B90, Unit/abilityownernotifygameui.cpp.
void __fastcall NotifyGameUIOfAbilityOwner(void* manager);

void CUnit::NotifyAbilityManagerAndGameUI(void* manager)
{
    ((SUnitAbilityInstallLeaf*)this)->InstallAbilityAgent(manager);
    NotifyGameUIOfAbilityOwner(manager);
}
