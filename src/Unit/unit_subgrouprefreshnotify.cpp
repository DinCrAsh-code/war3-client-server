//============================================================================
//  0x6F3329B0 - CUnit::NotifySubgroupChanged.
//
//  Called from CAbilityInventory::DetachAndNotify (abilityinventorydispatch.cpp)
//  as the very last step of a successful detach, on the unit whose slot
//  just emptied: fetch the game UI singleton (GetGameUI(0, 0), already
//  established - gameui.h) and, if it exists yet, hand `this` to its own
//  subgroup-notify member.
//
//  Own translation unit: a whole further subsystem sits behind that one
//  call (0x6F3008F0 - network-command construction for the multiplayer
//  selection-subgroup packets, itself opening onto ~1000 more instructions
//  across CNetCommandUnitRefreshSubGroup/CNetCommandUnitSelectSubGroup and
//  their own unreproducible SEH frames), squarely the kind of "genuinely
//  unbounded, unrelated further subsystem" this session's own instructions
//  say to stop at rather than chase - so 0x6F3008F0 stays a documented
//  naked redirect below, and this function itself is the boundary.
//============================================================================
#include "unit.h"
#include "gameui.h"

//----------------------------------------------------------------------------
//  0x6F3008F0 - the game UI's own subgroup-changed notifier.  `this` is
//  whatever GetGameUI(0, 0) returns, reinterpreted the same way every
//  other file in src/GameUI casts it to whichever narrow view it needs
//  (gameuicursorcolor.cpp's own CScreenFrame* cast is the model for this).
//  `retn 4` read off this target's own dump; not reconstructed - see this
//  file's own header for why.
//----------------------------------------------------------------------------
struct SGameUISubgroupNotify
{
    void NotifySubgroupChanged(void* subject);
};

__declspec(naked) void SGameUISubgroupNotify::NotifySubgroupChanged(void*)
{
    __asm
    {
        mov     eax, 06F3008F0h
        jmp     eax
    }
}

void CUnit::NotifySubgroupChanged()
{
    SGameUISubgroupNotify* ui = (SGameUISubgroupNotify*)GetGameUI(0, 0);
    if (ui)
        ui->NotifySubgroupChanged(this);
}
