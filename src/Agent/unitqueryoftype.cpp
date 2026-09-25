//============================================================================
//  0x6F46D1B0 - EnumerateUnitsOfType: look a four-character type id up in
//  the agile type registry and walk every live instance of it.
//
//  The third of BuildAndRegisterEventBinding's three enumerators
//  (agenteventbinding.cpp), and the one it falls back on when the query
//  names neither a circle nor a rectangle - so "every unit of this type,
//  wherever it is".  Its own `zero` argument is the recursion switch
//  EnumerateTypeInstances takes, and that one call site passes 0: no child
//  types.
//
//  The lookup is the AGILE_TYPE_ID / Hash / Ptr chain every other reader of
//  the registry uses, spelled the way jassrelationagent.cpp spells it - the
//  named id whose address is taken twice, the singleton read into its own
//  local before the hash so it survives the call in a callee-saved
//  register, and the hash in a local so it is computed before the argument
//  pushes rather than between them.  The registry itself travels on into
//  the walk, which needs it to resolve child types.
//
//  Own translation unit: it calls EnumerateTypeInstances for real.
//============================================================================
#include "agiletypeinstances.h"

void __fastcall EnumerateUnitsOfType(unsigned int typeId,
                                     UnitQueryVisitFn fn,
                                     void* ctx, int recurse)
{
    AGILE_TYPE_ID id(typeId);
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();

    //  The record in a named local, not inside the argument list: written
    //  inline the lookup happens *between* the outer call's pushes, where
    //  the shipped stream has it finished before the first of them.
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    EnumerateTypeInstances(type, fn, ctx, game, recurse);
}
