//============================================================================
//  The JASS game-state singleton's seven owner-relationship masks, tags
//  16..23 of BuildAndRegisterEventBinding's variadic block
//  (agenteventbinding.cpp).  Redirect thunks in
//  agenteventbinding_thunks.cpp until now.
//
//      0x6F3A3490  PlayerMask       relation field +0x38
//      0x6F3A3780  SelectableMask                  +0x48
//      0x6F3A37A0  SlotMatches                     +0x58
//      0x6F3A3510  AllyMask                        +0x68
//      0x6F3A3730  EnemyMask                       +0x78
//      0x6F3A35A0  VisibleMask                     +0x98
//      0x6F3A3680  DetectedMask                    +0xA8, plus +0x98
//
//  All seven are the same three steps: index the singleton's own array by
//  the player, make-or-fetch that player's relation agent
//  (jassrelationagent.cpp), and read one of its handle-ref fields.  Only
//  the field offset differs, which is why the seven are one file rather
//  than seven - they do not call each other, so nothing here can be
//  inlined into anything that has to stay a real call.
//
//  **The array indexing is `this`'s own.**  The shipped bodies push the
//  player and `call 0x6F3A1650` without touching ecx, so the receiver is
//  the singleton itself: CJassGameStateMasks and
//  IndexedArrayHolder_6F3A1650 are two views of the one object at
//  dword_6FAB65F4.  `At` is called through a cast to that class rather
//  than re-declared as a member here, because MSVC puts the class name in
//  the mangled name and a second declaration would be a second symbol -
//  an LNK2019 that verify.py cannot see, per
//  docs/notes/verifier-gate-link-failures.md.
//
//  The masks are `unsigned int` but QueryField78 (queryfield.cpp) returns
//  `int`; the cast is at the return, where the shipped code has nothing at
//  all, because the two are the same 32 bits.
//============================================================================
#include "agenteventbinding.h"
#include "jassrelationagent.h"

//  Re-declared to match misc_field_getters.cpp's own struct exactly, so
//  this call site mangles the same way and reaches the real symbol.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  The record for `player`, then its agent.
//
//  **A static helper and deliberately not a macro.**  Textually substituting
//  the same expression into the six one-line masks costs each of them its
//  match: MSVC then reloads `this` into ecx after the `At` call
//  (`mov ecx, eax` where the shipped code has none) and the score drops to
//  0.824 across all six.  Inlined from a function it gets the shipped
//  register assignment exactly.  The price is one out-of-line COMDAT copy
//  in the listing that nothing calls; it has internal linkage, so it is
//  never a hook candidate, and /OPT:REF drops it at link time.
static SJassRelationAgent* RelationAgentFor(CJassGameStateMasks* self,
                                            int player)
{
    return ((SJassPlayerRelations*)
            ((IndexedArrayHolder_6F3A1650*)self)->At(player))
                ->EnsureRelationAgent();
}

unsigned int CJassGameStateMasks::PlayerMask(int player)
{
    return (unsigned int)RelationAgentFor(this, player)->m_player.QueryField78();
}

unsigned int CJassGameStateMasks::SelectableMask(int player)
{
    return (unsigned int)RelationAgentFor(this, player)->m_selectable.QueryField78();
}

unsigned int CJassGameStateMasks::AllyMask(int player)
{
    return (unsigned int)RelationAgentFor(this, player)->m_ally.QueryField78();
}

unsigned int CJassGameStateMasks::EnemyMask(int player)
{
    return (unsigned int)RelationAgentFor(this, player)->m_enemy.QueryField78();
}

unsigned int CJassGameStateMasks::VisibleMask(int player)
{
    return (unsigned int)RelationAgentFor(this, player)->m_visible.QueryField78();
}

//----------------------------------------------------------------------------
//  0x6F3A37A0 - is bit `value` of the slot mask set.
//
//  `1 << value` and not a table: the shipped code really does
//  `mov edx, 1` / `shl edx, cl`, which is the variable shift and not a
//  constant one.  The neg/sbb/neg triple at the tail is this compiler's
//  "normalise to 0 or 1", i.e. a `!= 0` in the return expression rather
//  than an `if`.
//----------------------------------------------------------------------------
int CJassGameStateMasks::SlotMatches(int slot, int value)
{
    return (RelationAgentFor(this, slot)->m_slot.QueryField78() &
            (1 << value)) != 0;
}

//----------------------------------------------------------------------------
//  0x6F3A3680 - the one mask that is two fields.
//
//  Take +0xA8, and when the game-state block's own +0x38 has bit 0x10000
//  set, OR in +0x98 as well.  **The relation agent is fetched twice**, once
//  per field: the shipped code keeps the *record* in ebx across the flag
//  test and calls 0x6F41B420 again rather than keeping its answer, so the
//  second `->EnsureRelationAgent()` here is deliberate and not a missed
//  common subexpression.
//----------------------------------------------------------------------------
unsigned int CJassGameStateMasks::DetectedMask(int player)
{
    SJassPlayerRelations* record = (SJassPlayerRelations*)
        ((IndexedArrayHolder_6F3A1650*)this)->At(player);

    SJassRelationAgent* agent = record->EnsureRelationAgent();
    unsigned int mask = (unsigned int)agent->m_detected.QueryField78();
    //  `> 0` and not `!= 0`.  The two are the same test on an unsigned
    //  word, but MSVC spells the first with `ja`/`jbe` and the second with
    //  `jne`/`je`, and the shipped code has `jbe`.
    if ((m_pGameState->m_flags38 & 0x10000) > 0)
    {
        SJassRelationAgent* again = record->EnsureRelationAgent();
        mask |= (unsigned int)again->m_visible.QueryField78();
    }
    return mask;
}
