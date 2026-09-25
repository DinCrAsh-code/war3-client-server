//============================================================================
//  0x6F314A70 - ResolvePlayerColorIndex.
//
//  Which entry of the colour table a player's name should be drawn from,
//  given the palette slot that player would otherwise get.  Every refusal
//  hands the caller's own fallback straight back, which is why the shipped
//  code puts it in eax before the first test and every early exit shares one
//  epilogue - written here as a single `goto` target for the reason
//  docs/msvc-vc8-idioms.md records under "Shared exits need `goto`".
//
//  The twelve-iteration scan asks the relation table *both* ways round
//  (player -> i and i -> player), which is what the two calls with their
//  two arguments swapped are; the first match wins and is tail-called into
//  ResolvePlayerColorSlot.
//
//  Own translation unit: five real calls out of it, one of them to
//  ResolvePlayerColorSlot in its own shipped module.
//============================================================================
#include "gameui.h"
#include "playercolor.h"        // ResolvePlayerColorSlot, g_unk6FAB65F4

//  See gameuiplayername.cpp: the world object's own array accessor, already
//  reconstructed, declared locally rather than re-thunked.
struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
};

//: dword_6FAB4FC0 - set while the game is not using team colours at all.
extern int g_teamColorsDisabled;
//: dword_6FAB4FBC / 4FB8 / 4FB4 / 4FB0 - the four fixed entries the local
//: player, an ally, a neutral and an enemy are drawn in.
extern unsigned int g_colorIndexSelf;
extern unsigned int g_colorIndexAlly;
extern unsigned int g_colorIndexNeutral;
extern unsigned int g_colorIndexEnemy;

enum { kPlayerSlotCount = 16, kRelationSlotCount = 12 };

unsigned int __fastcall ResolvePlayerColorIndex(unsigned int player,
                                                unsigned int fallback)
{
    unsigned int result = fallback;

    if (player >= kPlayerSlotCount)
        goto done;
    if (player == kPlayerSlotCount - 1)
        goto done;
    if (g_teamColorsDisabled != 0)
        goto done;

    {
        CGameWorld* world = (CGameWorld*)g_unk6FAB65F4;

        if (world == 0)
            goto done;

        unsigned int localSlot =
            ((IndexedArrayHolder_6F3A1650*)world)->m_actingSlot;

        if (localSlot >= kPlayerSlotCount)
            goto done;

        for (unsigned int i = 0; i < kRelationSlotCount; ++i)
        {
            if (world->TestRelationBit(player, (int)i)
                && world->TestRelationBit(i, (int)player))
                return ResolvePlayerColorSlot(i);
        }

        if (player == localSlot)
            return g_colorIndexSelf;

        if (world->TestRelationBit(localSlot, (int)player))
            return g_colorIndexAlly;

        return player >= kRelationSlotCount ? g_colorIndexNeutral
                                            : g_colorIndexEnemy;
    }

done:
    return result;
}
