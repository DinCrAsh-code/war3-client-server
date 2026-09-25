//============================================================================
//  0x6F41B5A0 - every hero this player owns.
//
//  One enumeration of agile type 'AHer' with a two-word context - the
//  player index and the array to fill - and the visitor
//  (0x6F419F80, not reconstructed) does the rest.  The array is returned
//  **by value**: the shipped `retn 4` with `this` in ecx and the buffer as
//  the only stack word is the thiscall class-return shape, and the trailing
//  `mov eax, esi` is the buffer handed back.
//
//  The four zero stores are the array's own construction, written out
//  rather than left to TSGrowableArray's implicit constructor - the shipped
//  code stores all four before it touches anything else, which is exactly
//  what a zeroing default constructor on the return buffer gives.
//
//  What this build cannot reproduce is the frame: the shipped function
//  carries VC8's unified `__try` frame for the array's own destructor on an
//  exceptional exit, which this repo's fixed `/GS- /EHs-c-` never emits
//  (docs/msvc-vc8-idioms.md).
//============================================================================
#include "playerheroeval.h"
#include "agenteventbinding.h"
#include "agent.h"

//  The enumeration context, as the shipped code lays it out on the stack:
//  the player index first and the array to fill after it.
struct SHeroCollectContext
{
    int                         m_playerIndex;   // +0x00
    TSGrowableArray<CAgentPtr>* m_out;           // +0x04
};

TSGrowableArray<CAgentPtr> SPlayerHeroes::CollectHeroes()
{
    TSGrowableArray<CAgentPtr> heroes;
    heroes.m_alloc = 0;
    heroes.m_count = 0;
    heroes.m_data = 0;
    heroes.m_chunk = 0;

    SHeroCollectContext context;
    context.m_playerIndex = m_playerIndex;
    context.m_out = &heroes;

    EnumerateUnitsOfType(HeroAgileTypeId(), HeroCollectVisitor, &context, 0);

    return heroes;
}
