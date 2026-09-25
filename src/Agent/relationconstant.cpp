//============================================================================
//  0x6F314BB0 - given two JASS player slots, which of three fixed constants
//  describes slot2's relationship to slot1 for UI purposes. Its one caller
//  in this repo's dumps, CUnit's own slot 100 override (sub_6F285770,
//  not yet reconstructed), compares the result against the "ColorFriend"
//  config color to decide between "ColorFriend" and "ColorNeutral" for the
//  unit's selection circle.
//
//  A **free function returning a class by value**: `ecx` on entry is the
//  hidden return buffer, `edx` the first real argument (slot1), the second
//  (slot2) on the stack, `retn 4` - CFloat.h's own "hidden pointer with a
//  user-declared copy constructor" convention, here for a class that is
//  nothing but one `int`. GetOrCreateFootprintTypeData-shaped naming
//  applies: the constant's own meaning past "which UI bucket" is not
//  reachable from this call tree, so the three globals are named for what
//  the checks that select them test rather than guessed at from wider
//  context.
//
//  **`allied` is read into a local once, ahead of both checks that can
//  return it** (record1's own +0xF0 flag, and record1's ally-agent mask
//  bit for slot2) - the shipped `mov eax, dword_6FAB5968` sits at the very
//  top of the function, and both matching `retn 4` arms reuse the one
//  cached copy (`[esp+var_4]`) rather than re-reading the global.
//============================================================================
#include "jassrelationagent.h"
#include "relationconstant.h"

//  Re-declared to match misc_field_getters.cpp's own struct exactly (same
//  reasoning jassgamestatemasks.cpp already gives): `At()`'s real
//  definition lives in a different TU, and a second declaration has to
//  mangle identically to reach it rather than becoming its own symbol.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

extern int g_relationConstAllied;    // dword_6FAB5968
extern int g_relationConstVisible;   // dword_6FAB5964
extern int g_relationConstDefault;   // dword_6FAB596C

SRelationColorConstant __fastcall GetPlayerRelationConstant(int slot1, int slot2)
{
    int allied = g_relationConstAllied;

    IndexedArrayHolder_6F3A1650* holder =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    SJassPlayerRelations* record1 =
        (SJassPlayerRelations*)holder->At(slot1);
    SJassPlayerRelations* record2 =
        (SJassPlayerRelations*)holder->At(slot2);

    if (record1->m_fieldF0.QueryField78())
        return SRelationColorConstant(allied);

    if ((unsigned int)record2->EnsureRelationAgent()->m_visible.QueryField78()
            & (1 << slot1))
        return SRelationColorConstant(g_relationConstVisible);

    if ((unsigned int)record1->EnsureRelationAgent()->m_player.QueryField78()
            & (1 << slot2))
        return SRelationColorConstant(allied);

    return SRelationColorConstant(g_relationConstDefault);
}
