//============================================================================
//  0x6F314A30 - which of the game's colour slots a player index maps to.
//
//  Own translation unit: CWar3Image::SetOwningPlayer (0x6F26B8C0) and
//  CSprite::ApplyTeamColorForPlayer (0x6F314CC0) both call it for real.
//
//  Players 16 and up (the neutral/observer range) take the last slot
//  outright.  Below that the player's own recorded colour wins if the slot
//  table is up and the player has one that is not -1, otherwise the index
//  stands in for it - and either way the result is taken modulo the slot
//  count, which is the `xor edx,edx / div` at the end and is why the
//  arithmetic here is unsigned.
//============================================================================
#include "playercolor.h"

//----------------------------------------------------------------------------
//  DEMO TOGGLE - off, and it must stay off on main.
//
//  Defining this makes every widget in the game render in one player's
//  colour regardless of who owns it.  It exists to demonstrate that the
//  JMP hooks really do take over the running game's behaviour and not
//  merely coexist with it: with it off, the verifier's tracepoint on
//  SetSpriteColorSlot (0x6F314C80) captures slot 0 on the single-player
//  test map; with it on, every one of those same calls captures 5 instead,
//  and the value can only have come from this build, because this is the
//  only function that computes it.
//
//  It is a *deliberate* behavioural difference, so it must never be
//  compiled into anything the pipeline scores or archives - this repo is a
//  matching decompilation and a target's BEHAVIOUR verdicts would be lies
//  with it on.  It changes nothing when undefined, so verify.py's output is
//  byte-identical either way.
//
//  To turn it on, uncomment the #define below and re-run the verifier gate.
//  A /D on the command line does not reach it: the gate builds on the
//  operator's machine from the pushed commit, with that machine's own
//  flags, so the switch has to be in the source to travel.
//----------------------------------------------------------------------------
//#define MDP_DEMO_FORCE_PLAYER_COLOUR 5
//  0x6F3A1650 - already reconstructed as IndexedArrayHolder_6F3A1650::At
//  (Misc/misc_field_getters.cpp).  Re-declared, not re-thunked: a second
//  naked function at the same address is two symbols for one function, and
//  it is what kept this call site on the shipped code (see playercolor.h).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

unsigned int __fastcall ResolvePlayerColorSlot(unsigned int player)
{
#ifdef MDP_DEMO_FORCE_PLAYER_COLOUR
    //  Kept inside the modulo the real function ends with, so the result is
    //  in range whatever the live slot count turns out to be - a raw
    //  constant could index past the colour table and crash.
    return (unsigned int)(MDP_DEMO_FORCE_PLAYER_COLOUR) % g_playerColorSlotCount;
#else
    if (player >= 0x10)
        return g_playerColorSlotCount - 1;

    unsigned int color = player;
    if (g_unk6FAB65F4 != 0)
    {
        SPlayerSlot* slot = (SPlayerSlot*)
            ((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(player);
        if (slot != 0 && slot->m_color != 0xFFFFFFFF)
            color = slot->m_color;
    }
    return color % g_playerColorSlotCount;
#endif
}
