//============================================================================
//  SJassPlayerRates::SetCrippled (0x6F416670) - CripplePlayer's own
//  (jassnatives_misc.cpp) worker.  Both arms walk every unit of the
//  "player-controlled unit" agile type (or a second, "fixed" agile type on
//  the disable path) through EnumerateUnitsOfType
//  (Agent/agenteventbinding.h) with a small per-call context block; neither
//  visitor callback is reached by anything else this batch dumps, so both
//  stay naked redirects to their real, unhooked bodies.
//============================================================================
#include "jassnatives.h"
#include "agenteventbinding.h"
#include "gameconfig.h"

//  0x6F0D2650 - Misc/fixedagiletypeid_42707276.cpp's own free function.
unsigned int GetFixedAgileTypeId_42707276();

//  0x6F26C1C0 - Misc/misc_rawcode_getters.cpp's own free function, '+w3u'.
unsigned int GetUnitTypeFourCC();

//  0x6F4165A0 - the enable-path visitor.  No call-tree dump reaches it (it
//  is taken only by address, for EnumerateUnitsOfType's own callback
//  argument), so it stays a naked redirect.
__declspec(naked) int __fastcall CrippleEnableVisitor(void*, void*)
{
    __asm { mov eax, 06F4165A0h }
    __asm { jmp eax }
}

//  0x6F416600 - the disable-path visitor, same reasoning.
__declspec(naked) int __fastcall CrippleDisableVisitor(void*, void*)
{
    __asm { mov eax, 06F416600h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F416670 - `retn 8`, `this` (the player, cast the same way
//  jassnatives_misc.cpp's CripplePlayer already does) in ecx.
//----------------------------------------------------------------------------
void SJassPlayerRates::SetCrippled(int crippled, int flag)
{
    SJassPlayer* player = (SJassPlayer*)this;

    if (crippled)
    {
        //  var_8/var_4/var_2: one contiguous 8-byte block, in this order.
        struct
        {
            unsigned int   worldField34;
            unsigned short playerIndex;
            unsigned short flag;
        } ctx;

        ctx.playerIndex = player->m_playerIndex;
        ctx.flag = (unsigned short)flag;
        ctx.worldField34 = *(unsigned int*)((char*)g_unk6FAB65F4 + 0x34);

        EnumerateUnitsOfType(GetUnitTypeFourCC(), CrippleEnableVisitor, &ctx, 1);

        player->m_field328 = (unsigned short)flag;
    }
    else
    {
        //  Reuses the incoming `flag` argument's own stack slot as the
        //  4-byte context block: its low word is overwritten with the
        //  player's own index and its address passed straight through.
        //  Spelled that way here too - taking `&flag` is what keeps this
        //  parameter in its stack home rather than a register, matching
        //  the shipped code's own `lea eax, [esp+arg_4]`.
        *(unsigned short*)&flag = player->m_playerIndex;

        player->m_field328 = 0;

        EnumerateUnitsOfType(GetFixedAgileTypeId_42707276(),
                             CrippleDisableVisitor, &flag, 1);
    }
}
