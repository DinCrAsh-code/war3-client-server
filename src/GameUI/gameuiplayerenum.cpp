//============================================================================
//  0x6F3A2820 and 0x6F3A3400 - the two members of the world-object singleton
//  this batch reconstructs.  Neither calls the other.
//
//  Both are `__thiscall` members whose receiver is `dword_6FAB65F4`, the
//  world object the rest of this repo reaches through
//  IndexedArrayHolder_6F3A1650 - which is why `this` is cast rather than
//  re-declared.  0x6F3A2820 never *reads* its receiver, but its caller sets
//  ecx before the call and the shipped code cleans one stack argument, which
//  is the "no-argument function called with a live ecx" shape
//  docs/msvc-vc8-idioms.md records: a member with an unused `this`, not a
//  free function.
//============================================================================
#include "gameui.h"
#include "agenteventbinding.h"      // EnumerateUnitsOfType, UnitQueryVisitFn
#include "jassrelationagent.h"      // SJassPlayerRelations::EnsureRelationAgent

extern void* g_unk6FAB65F4;

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
    int At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3A27D0 - the visitor 0x6F3A2820 hands to the enumerator, in the
//  0x6F3Axxxx module and not part of this batch.  A redirect, because the
//  *address* is what the enumerator calls: taking the address of a
//  reconstruction would be fine, but there is no body here to take it of.
//
//  `retn 0`, so the declaration below pushes nothing - settled rather than
//  assumed: `ida_query func_at 0x6F3A27D0` puts its range at
//  0x6F3A27D0..0x6F3A2814 and `ida_query bytes 0x6F3A280C 8` ends it
//  `5F B8 01 00 00 00 5E C3`.  This is the one entry in
//  tools/thunk_abi_audit.py's "no dump body" bucket this batch adds.
//----------------------------------------------------------------------------
int __fastcall SetTextAgentPaused(void* agent, void* context);

__declspec(naked) int __fastcall SetTextAgentPaused(void*, void*)
{
    __asm
    {
        mov     eax, 06F3A27D0h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F3A2820 - pause (or resume) every floating-text agent in the world.
//----------------------------------------------------------------------------
void CGameWorld::SetTextAgentsPaused(int paused)
{
    EnumerateUnitsOfType(GetTextAgentTypeId(), SetTextAgentPaused,
                         (void*)paused, 0);
}

//----------------------------------------------------------------------------
//  0x6F3A3400 - is one bit of a player's owner-relationship mask set?
//
//  `neg`/`sbb`/`neg` around the masked test is MSVC's 0/1 normalisation of
//  `!= 0`, not something the source spells; the shift is formed into a
//  register first because the mask is the second operand of the `and`.
//----------------------------------------------------------------------------
int CGameWorld::TestRelationBit(unsigned int player, int bit)
{
    int record = ((IndexedArrayHolder_6F3A1650*)this)->At(player);

    SJassRelationAgent* agent =
        ((SJassPlayerRelations*)record)->EnsureRelationAgent();

    unsigned int mask = (unsigned int)agent->m_player.QueryField78();

    return (mask & (1u << bit)) != 0;
}
