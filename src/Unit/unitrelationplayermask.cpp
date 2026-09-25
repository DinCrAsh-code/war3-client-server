//============================================================================
//  0x6F25D480 - CUnit::RelationPlayerMask: turn the owner-relationship half
//  of a unit-enumeration filter into the set of player slots the
//  enumeration must *exclude*.
//
//  Five bits of the filter name relationship classes, and the answer is
//  built from two of the relation agent's own published masks
//  (jassrelationagent.h): m_player, the slots that are real players at all,
//  and m_enemy, the ones this player is at war with.
//
//      0x2000  self          the receiver's own slot
//      0x4000  enemies       m_enemy & m_player
//      0x8000  allies        ~m_enemy & m_player
//     0x10000  everyone else ~m_player
//      0x1000  drops the receiver's own slot again on the way out
//
//  All five set at once is "no restriction", and the function says so
//  before it does any work.  `invert` is what every caller passes 1 for:
//  the enumeration mask's low word is a *reject* set, so the built mask is
//  complemented at the tail - which is also why the "no restriction" exit
//  answers 0 when inverting and 0xFFFF when not.
//
//  **`this` may be null.**  The filter can name the relationships without
//  naming a unit, in which case its own third word has to name the player;
//  with neither there is nothing to be related *to* and the answer is again
//  "no restriction".
//
//  **The three guards are written as `goto`s onto labelled blocks in the
//  shipped order**, and that is deliberate.  Two spellings were measured
//  first: assigning the parameter rather than a local costs `player` its
//  register (MSVC keeps it in its home slot and reloads it four times -
//  0.617), and nesting the guards as `if`/`else if` puts the virtual call
//  inline between them and needs an extra `jmp` past it (0.617 as well).
//  With a local and three forward gotos the two streams are the same
//  instructions in the same order; all that is left is that MSVC parks the
//  shared "no restriction" return at the *tail* of the function where the
//  shipped code has it between the guards and the body.  Spelling it as a
//  labelled block in that position (this file) does not move it - block
//  placement is the scheduler's.
//
//  The owning player comes through the real `virtual` slot 59 rather than a
//  hand-written dispatch, so tools/vtable_dispatch_audit.py has nothing new
//  to judge.
//
//  Own translation unit: FoldQueryClassMask calls it for real.
//============================================================================
#include "unit.h"
#include "jassrelationagent.h"

//  The player-slot table, and misc_field_getters.cpp's indexer over it -
//  re-declared to mangle identically rather than re-thunked, exactly as
//  unit_visibilitymask.cpp does.
extern void* g_unk6FAB65F4;

struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

unsigned int CUnit::RelationPlayerMask(unsigned int relations, int player,
                                       int invert)
{
    unsigned int mask = 0;
    int who = player;

    if ((relations & 0x0001F000) == 0x0001F000)
        goto unrestricted;
    if (this != 0)
        goto askTheUnit;
    if (who != -1)
        goto restricted;

unrestricted:
    return invert != 0 ? 0u : 0xFFFFu;

askTheUnit:
    who = GetOwningPlayerIndex();

restricted:
    {
        SJassRelationAgent* agent =
            ((SJassPlayerRelations*)
                ((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(who))
                    ->EnsureRelationAgent();

        unsigned int players = (unsigned int)agent->m_player.QueryField78();
        unsigned int enemies = (unsigned int)agent->m_enemy.QueryField78();

        if (relations & 0x4000)
            mask = enemies & players;
        if (relations & 0x8000)
            mask |= ~enemies & players;
        if (relations & 0x10000)
            mask |= ~players;

        unsigned int own = 1u << who;
        if (relations & 0x2000)
            mask |= own;
        else
            mask &= ~own;

        if (invert != 0)
        {
            mask = ~mask;
            if (relations & 0x1000)
                mask &= ~own;
        }

        return (unsigned short)mask;
    }
}
