//============================================================================
//  The nine JASS natives at 0x6F3BAE10 - 0x6F3BAF20 that read and write the
//  game-configuration block hanging off the world object at +0x30
//  (Game/gameconfig.h).
//
//  Every one of them is `__cdecl` with a bare `retn` - the registration
//  table's own convention - and every one of them starts with the same two
//  loads, `mov eax, dword_6FAB65F4` / `mov ecx, [eax+30h]`.  That is what
//  reaching a field through `g_pGameConfig` in a single expression
//  produces; naming the block in a local first costs nothing here because
//  each function touches exactly one field.
//
//  One translation unit: none of the nine calls another, and the only real
//  call in the module is SetGameSpeed's, which goes to a different one
//  (0x6F3DB270, Game/gameconfigspeed.cpp).
//============================================================================
#include "gameconfig.h"

//----------------------------------------------------------------------------
//  0x6F3BAE10 - `SetGameTypeSupported` "(Hgametype;B)V".
//
//  The `cmp [esp+arg_4], 0` comes *before* the singleton load in the shipped
//  code: MSVC hoists the branch's own test above the two loads both arms
//  share.  That is scheduling, not a different source order.
//----------------------------------------------------------------------------
void __cdecl JASS_SetGameTypeSupported(int gameType, int supported)
{
    //  The block is named once and the *argument* re-read in each arm -
    //  which is the way round the shipped code has it.  Reaching through
    //  g_pGameConfig inside both arms instead duplicates the two singleton
    //  loads and hoists the argument load above the branch.
    SGameConfig* config = g_pGameConfig;

    if (supported)
        config->m_gameTypesSupported |= gameType;
    else
        config->m_gameTypesSupported &= ~gameType;
}

//----------------------------------------------------------------------------
//  0x6F3BAE40 - `IsGameTypeSupported` "(Hgametype;)B".
//
//  `xor eax,eax` / `cmp eax,edx` / `sbb eax,eax` / `neg eax` is the
//  four-instruction branchless flag that only an *ordered* comparison
//  against a materialised zero produces - `0u < x`, not `x != 0`, which
//  folds to the shorter `neg`/`sbb`/`neg`
//  (docs/msvc-vc8-idioms.md, "More source shapes from the second stub
//  pass").
//----------------------------------------------------------------------------
int __cdecl JASS_IsGameTypeSupported(int gameType)
{
    return 0u < (g_pGameConfig->m_gameTypesSupported & gameType);
}

//----------------------------------------------------------------------------
//  0x6F3BAE60 - `GetGameTypeSelected` "()Hgametype;".
//
//  Bit 3 is masked out of the answer.  Nothing in this family sets or reads
//  that bit, so the mask is kept as the literal the shipped code carries
//  rather than named for a meaning this call tree does not establish.
//----------------------------------------------------------------------------
int __cdecl JASS_GetGameTypeSelected()
{
    return g_pGameConfig->m_gameTypeSelected & 0xFFFFFFF7;
}

//----------------------------------------------------------------------------
//  0x6F3BAEC0 - `GetGameSpeed` "()Hgamespeed;".
//----------------------------------------------------------------------------
int __cdecl JASS_GetGameSpeed()
{
    return g_pGameConfig->m_speed;
}

//----------------------------------------------------------------------------
//  0x6F3BAED0 - `SetGameSpeed` "(Hgamespeed;)V".
//
//  `test dword ptr [ecx+38h], 4000h` / `ja` is the same `0u < (x & mask)`
//  spelling as IsGameTypeSupported above, in an `if` rather than as a
//  value: MSVC folds the `cmp reg,0` of the ordered comparison into the
//  `test` and keeps the *unsigned above* condition.  `!= 0` would give
//  `jne`.
//
//  The one instruction pair that is not the shipped one is the argument
//  load: the shipped code re-uses `eax`, which held the world pointer and
//  is dead by then, where this build takes `edx`.  Four spellings were
//  measured - the block reached through the macro in both uses, the
//  argument named first, the flags word named first, and the early return
//  written as a positive `if` - and every one of them allocates `edx`.
//  Register choice, nothing else: the eight instructions are otherwise the
//  shipped ones in the shipped order.
//----------------------------------------------------------------------------
void __cdecl JASS_SetGameSpeed(int speed)
{
    SGameConfig* config = g_pGameConfig;

    if (0u < (config->m_flags & 0x4000))
        return;

    config->SetGameSpeed(speed);
}

//----------------------------------------------------------------------------
//  0x6F3BAEF0 - `SetGamePlacement` "(Hplacement;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetGamePlacement(int placement)
{
    g_pGameConfig->m_placement = (unsigned char)placement;
}

//----------------------------------------------------------------------------
//  0x6F3BAF00 - `GetGamePlacement` "()Hplacement;".
//----------------------------------------------------------------------------
int __cdecl JASS_GetGamePlacement()
{
    return g_pGameConfig->m_placement;
}

//----------------------------------------------------------------------------
//  0x6F3BAF10 - `SetGameDifficulty` "(Hgamedifficulty;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetGameDifficulty(int difficulty)
{
    g_pGameConfig->m_difficulty = (unsigned char)difficulty;
}

//----------------------------------------------------------------------------
//  0x6F3BAF20 - `GetGameDifficulty` "()Hgamedifficulty;".
//----------------------------------------------------------------------------
int __cdecl JASS_GetGameDifficulty()
{
    return g_pGameConfig->m_difficulty;
}
