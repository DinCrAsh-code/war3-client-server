//============================================================================
//  0x6F300CB0 - CGameUI::NotifyAbilityListenerOfOwner.  `this` in ecx, one
//  stack pointer (`retn 4`).  Reached from NotifyGameUIOfAbilityOwner
//  (0x6F0D1B90, Unit/abilityownernotifygameui.cpp).
//
//  The owner is compared against one of two things depending on the game
//  mode - the acting player's own "tracked ability" field (+0x1E0 of
//  whatever object its own +0x34 names, read through the
//  already-reconstructed GetField0x1E0_6F41FA00) when IsGameModeOne() is
//  false, or CGameUI's own +0x3B8 tracked-host id when it is true.  On a
//  match: fetch the item bar's own slot 1
//  (CGameUIItemBar::GetSlot0x12C, GameUI/gameuiitembarslot.cpp) and, if it
//  named something, tell that something's own +0x16C sub-object about it
//  (NotifyResolved(0)).  Then, regardless: ask one of two near-identical
//  zero-argument queries on `this` depending on the "pause the command
//  card while typing" preference (m_option1D0, gameui.h) and tail-call
//  NotifyResolved(0) again on *its* own +0x150 sub-object - the shipped
//  `jmp sub_6F35DD00` reusing this function's own single stack argument
//  slot as that tail call's own argument.  Three callees (0x6F338830's
//  own neighbours - 0x6F35DD00/0x6F2F4820/0x6F2F4790) are debug-assertion-
//  heavy CGameUI internals well outside this batch's own scope and are
//  kept redirects rather than guessed at.
//============================================================================
#include "gameui.h"

//  Re-declared to match Unit/unit_abilityhostnotify.cpp's own definition
//  exactly.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

extern void* g_unk6FAB65F4;   // dword_6FAB65F4, the world-object singleton
int __fastcall GetField0x1E0_6F41FA00(const void* self);   // Misc/trivialaccessors_04.cpp
int __fastcall IsGameModeOne();                            // Game/gamemode.cpp

//  A sub-object at +0x16C/+0x150 of two different receivers, both reached
//  the same way: one stack arg (`retn 4`).
struct SGameUIAbilityListenerCallees
{
    void NotifyResolved(void* arg);         // 0x6F35DD00, retn 4
};

void CGameUI::NotifyAbilityListenerOfOwner(void* owner)
{
    if (owner == 0)
        return;

    unsigned int actingSlotWide =
        *(unsigned short*)((char*)g_unk6FAB65F4 + 0x28);
    IndexedArrayHolder_6F3A1650* world = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    void* actingPlayer = (void*)world->At(actingSlotWide);
    void* trackedModule = *(void**)((char*)actingPlayer + 0x34);
    int trackedValue = GetField0x1E0_6F41FA00(trackedModule);

    if (!IsGameModeOne())
    {
        if ((int)owner != trackedValue)
            return;
    }
    else
    {
        if (owner != (void*)(unsigned int)m_trackedAbilityHostId)
            return;
    }

    CGameUI* ui2 = (CGameUI*)GetGameUI(0, 0);
    if (ui2 == 0)
        return;

    void* fetched = ui2->m_pItemBar->GetSlot0x12C(1);
    if (fetched != 0)
    {
        void* sub16C = *(void**)((char*)fetched + 0x16C);
        ((SGameUIAbilityListenerCallees*)sub16C)->NotifyResolved(0);
    }

    if (m_option1D0 != 0)
    {
        void* result = ui2->QueryFollowUpTarget();
        void* sub150 = *(void**)((char*)result + 0x150);
        ((SGameUIAbilityListenerCallees*)sub150)->NotifyResolved(0);
    }
    else
    {
        void* result = ui2->QueryFollowUpTargetAlt();
        void* sub150 = *(void**)((char*)result + 0x150);
        ((SGameUIAbilityListenerCallees*)sub150)->NotifyResolved(0);
    }
}

//----------------------------------------------------------------------------
//  Thunks for the callees this file declares and calls but does not
//  reconstruct.  See the file header for why each stays a redirect.
//----------------------------------------------------------------------------
#define GAMEUI_ABILITY_THUNK(addr)  \
    {                               \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

__declspec(naked) void SGameUIAbilityListenerCallees::NotifyResolved(void*)   GAMEUI_ABILITY_THUNK(0x6F35DD00)
__declspec(naked) void* CGameUI::QueryFollowUpTarget()      GAMEUI_ABILITY_THUNK(0x6F2F4820)
__declspec(naked) void* CGameUI::QueryFollowUpTargetAlt()   GAMEUI_ABILITY_THUNK(0x6F2F4790)
