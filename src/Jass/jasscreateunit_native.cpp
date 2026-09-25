//============================================================================
//  0x6F3C5230 - JASS_CreateUnit, the `CreateUnit` native's own entry point.
//
//  Two independent pieces of ground truth fix this function, and they agree
//  with each other:
//
//   * the registration table at 0x6F3D4020 registers it as
//
//         push offset aHplayerIrrrHun; "(Hplayer;IRRR)Hunit;"
//         mov  edx, offset aCreateunit; "CreateUnit"
//         mov  ecx, offset sub_6F3C5230
//         call sub_6F455110
//
//     i.e. (player handle, integer, real, real, real) -> unit handle, and
//     the bare `retn` makes it __cdecl - the same shape every other native
//     in this module has.  See docs/targets/jass-natives-registration-table.md;
//
//   * `asm/processed/sub_6F430C80_0x6F430C80_calltree_asm.md`'s own
//     `## User notes` carry a hand-decompiled listing of this exact
//     function, verified in a debugger by the operator, with every callee's
//     prototype checked by hand.
//
//  The three `R` arguments arrive as *pointers* to encoded CFloat values,
//  which is the JASS-native convention for reals throughout this module
//  (jassnatives.h), and the `Hunit;` result is the agent-slot token
//  CAgentRegistry::Register hands back - exactly what JASS_UnitAddItemById
//  already returns for an item.
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"
#include "agentregistry.h"
#include "jasscreateunit.h"

//  dword_6FAAE5E8 - the degrees-to-radians factor, already named by
//  jasssetitemposition.cpp for the same reason: `facing` crosses the JASS
//  boundary in degrees and the simulation wants radians.
extern const CFloat g_CFloatDegToRad;

int __cdecl JASS_CreateUnit(int hPlayer, int unitId,
                            const CFloat* posX, const CFloat* posY,
                            const CFloat* facing)
{
    JassNativePrologue();

    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    //  Copy-initialised, not default-constructed then assigned: the shipped
    //  prologue is a bare `push ecx`, which is MSVC's "give me one dword of
    //  frame" idiom and stores nothing - the pseudocode's own `a1 = v5 /*
    //  ecx */` is that push, not an initialisation.  A default-constructed
    //  CFloat would zero the slot first (CFloat.h), one instruction the
    //  shipped code does not have, and an assignment would build the
    //  product in a temporary and copy it here instead of letting
    //  operator*'s hidden return buffer be this slot.
    CFloat radians = *facing * g_CFloatDegToRad;

    //  The player index is the byte at +0x30, widened - the shipped code
    //  reads it with a `movzx`, which is what SJassPlayer::m_playerIndex's
    //  `unsigned char` spelling produces (jassnatives.h).
    //
    //  Named, not passed in place, and that is the whole difference between
    //  0.853 and 1.000.  Written inline the compiler loads posY into ecx
    //  first, then has to displace it for the fastcall argument and reads
    //  posX and the unit type out of the other two registers in the other
    //  order; naming it lets the value settle into ecx before any of the
    //  three pushes and frees eax to carry posX across `push eax`, which is
    //  the shipped schedule exactly.
    int playerIndex = player->m_playerIndex;

    CUnit* unit = CreateUnitForPlayer(playerIndex, unitId,
                                      posX, posY, &radians);

    //  Same tail as JASS_UnitAddItemById (jassunitadditembyid_native.cpp),
    //  instruction for instruction: the holder is *named* before the call
    //  rather than used in place, because the shipped code issues
    //  GetSlotTable before pushing Register's own two arguments, and that
    //  is the order a named receiver forces (MSVC evaluates arguments
    //  right-to-left and the object expression last).
    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    //  CUnit derives from CAgent at offset zero (unit.h, agent.h), so the
    //  cast is a re-spelling and not an adjustment - Register wants the
    //  agent view of the same object.
    return (int)registry->Register((CUnitAgent*)unit, 0);
}
