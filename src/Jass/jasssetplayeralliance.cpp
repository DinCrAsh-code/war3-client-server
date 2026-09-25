//============================================================================
//  JASS_SetPlayerAlliance (0x6F3C1050) - "(Hplayer;Hplayer;Halliancetype;B)V".
//  Sets or clears one alliance-relationship bit between two players, and
//  when the changed relationship is one of the two "shared vision" values
//  (5, 9), re-evaluates the source player's own controller-notify state and
//  the *other* player's leave-cleanup notify, then - only in a running
//  multiplayer game (world+0x3E0 nonzero) - refreshes the single
//  world-global visibility-mask state (Agent/jassrelationagent.h's
//  SWorldVisibilityMaskState, reached through world+0x34).
//============================================================================
#include "jassnatives.h"
#include "jassrelationagent.h"
#include "playerwar3.h"

//  0x6F41B4C0 - Player/playerwar3_handleplayerleftnotify.cpp's own naked
//  redirect; re-declared identically so this call site mangles to the same
//  symbol.
void __fastcall CPlayerWar3FinishLeaveNotify(CPlayerWar3*);

//----------------------------------------------------------------------------
//  Naked thunks for the three worker calls this closure needs beyond
//  EnsureRelationAgent/RefreshPlayerWorldVisibilityMasks above (see
//  jassrelationagent.h's own comments on each for why).
//----------------------------------------------------------------------------
__declspec(naked) void SJassRelationAgent::EnableAllianceFlag(int, int)
{
    __asm { mov eax, 06F3E6890h }
    __asm { jmp eax }
}

__declspec(naked) void SJassRelationAgent::DisableAllianceFlag(int, int)
{
    __asm { mov eax, 06F3E69E0h }
    __asm { jmp eax }
}

__declspec(naked) void SWorldVisibilityMaskState::TailCleanup()
{
    __asm { mov eax, 06F016CD0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F3C1050 - `__cdecl`, bare `retn`.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerAlliance(int hPlayer, int hOtherPlayer,
                                    int allianceType, int value)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return;

    SJassPlayer* other = ResolvePlayerHandle(hOtherPlayer);

    if (!other)
        return;

    int otherIndex = other->m_playerIndex;

    SJassRelationAgent* agent = ((SJassPlayerRelations*)player)->EnsureRelationAgent();

    if (value)
        agent->EnableAllianceFlag(otherIndex, allianceType);
    else
        agent->DisableAllianceFlag(otherIndex, allianceType);

    if (allianceType == 5 || allianceType == 9)
    {
        NotifyPlayerControllerChanged(player);
        CPlayerWar3FinishLeaveNotify((CPlayerWar3*)other);
    }

    if (*(int*)((char*)g_unk6FAB65F4 + 0x3E0))
    {
        SWorldVisibilityMaskState* worldState =
            *(SWorldVisibilityMaskState**)((char*)g_unk6FAB65F4 + 0x34);

        worldState->RefreshPlayerWorldVisibilityMasks();
        worldState->TailCleanup();
    }
}
