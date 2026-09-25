//============================================================================
//  The two objects the JASS game-state singleton's owner-relationship masks
//  walk through: the per-player record it indexes, and the relation agent
//  that record makes on first use.
//
//  Both are reached only from jassgamestatemasks.cpp and
//  jassrelationagent.cpp, and neither is a class this repo has recovered a
//  name for - the layout is exactly the fields those two files touch, per
//  CLAUDE.md's rule that struct offsets are exact and only touched members
//  get names.
//============================================================================
#ifndef JASSRELATIONAGENT_H
#define JASSRELATIONAGENT_H

#include "agenttypedslots.h"
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  The relation agent.  Eight handle-ref fields, 0x10 bytes apart from
//  +0x38, each holding one owner-relationship mask - the same
//  {pad, handle, typeTag} shape HandleRefFieldOwner_6F473170 already is, so
//  each is read with that class's own QueryField78 (queryfield.cpp) rather
//  than with a second copy of the same two loads.
//
//  Which offset is which relationship comes from the seven mask functions,
//  one apiece; +0x88 is the one no caller in this repo reaches and it is
//  named for its offset for that reason.
//----------------------------------------------------------------------------
struct SJassRelationAgent
{
    char                         m_reserved00[0x38];
    HandleRefFieldOwner_6F473170 m_player;        // +0x38
    HandleRefFieldOwner_6F473170 m_selectable;    // +0x48
    HandleRefFieldOwner_6F473170 m_slot;          // +0x58
    HandleRefFieldOwner_6F473170 m_ally;          // +0x68
    HandleRefFieldOwner_6F473170 m_enemy;         // +0x78
    HandleRefFieldOwner_6F473170 m_reserved88;    // +0x88
    HandleRefFieldOwner_6F473170 m_visible;       // +0x98
    HandleRefFieldOwner_6F473170 m_detected;      // +0xA8

    //  0x6F3E6890/0x6F3E69E0 - SetPlayerAlliance's own worker pair
    //  (Jass/jasssetplayeralliance.cpp): set or clear `otherIndex`'s own bit
    //  in one of the eight handle-ref fields above (which one is picked by
    //  `allianceType`, `<<4` into the +0x38 array), touching a ninth,
    //  currently-unnamed handle-ref field inside this struct's own
    //  `m_reserved00[0x38]` along the way and, on some `allianceType`
    //  values, notifying a widget-fog-refresh/hero-eval pair this batch's
    //  own worklist does not reach.  Both `retn 8`, `this` in ecx; kept
    //  naked redirects rather than a guess at that ninth field's real
    //  identity or at the two notify targets' own signatures.
    void EnableAllianceFlag(int otherIndex, int allianceType);
    void DisableAllianceFlag(int otherIndex, int allianceType);
};

//----------------------------------------------------------------------------
//  One player's record inside the game-state singleton's own array.  Only
//  the 'ally'-typed slot at +0x38 is touched here; item_selected.cpp names
//  the +0x34 of the same block for its own use and nothing here reaches it.
//----------------------------------------------------------------------------
struct SJassPlayerRelations
{
    char             m_reserved00[0x38];
    SCheckedAllySlot m_allySlot;      // +0x38
    char             m_reserved3C[0xF0 - 0x3C];
    //  +0xF0 - a {pad, handle, typeTag} handle ref on the *record itself*
    //  (not through EnsureRelationAgent()), read by sub_6F314BB0
    //  (relationconstant.cpp) ahead of anything else it looks at. Nothing
    //  in this repo's call trees writes it, so what the referenced object
    //  is stays unknown; named for its offset like +0x88 above.
    HandleRefFieldOwner_6F473170 m_fieldF0;   // +0xF0

    //  0x6F41B420 - make the relation agent if the slot is empty, then hand
    //  the slot's contents back.  `retn 0`: the record is the only argument
    //  and it arrives in ecx.
    SJassRelationAgent* EnsureRelationAgent();
};

//----------------------------------------------------------------------------
//  A second, single, world-global object at dword_6FAB65F4's own +0x34 -
//  *not* a per-player record (SetPlayerAlliance, jasssetplayeralliance.cpp,
//  reaches it through the world object directly, never through
//  ResolvePlayerHandle).  Only the one member this batch's call trees reach
//  is named.
//----------------------------------------------------------------------------
struct SWorldVisibilityMaskState
{
    char             m_reserved00[0x24];
    //  +0x24 - a flags byte.  Bit 0, tested by
    //  RefreshPlayerWorldVisibilityMasks (Jass/jassrefreshworldmasks.cpp),
    //  gates that function's own "am I the local player" fast path; nothing
    //  else in this batch's call trees reads or writes it.
    unsigned char    m_field24;        // +0x24
    char             m_reserved25[0x3C - 0x25];
    //  +0x3C/+0x3E/+0x40 - three words RefreshPlayerWorldVisibilityMasks
    //  latches from the local player's own +0x2E0 world-visibility record
    //  (the same SPlayerWorldMaskRecord Unit/unit_ownerworldmask.cpp reads):
    //  m_field3C either 0xFFFF, 0xFFF or `1 << localPlayerIndex` depending
    //  on two flag checks; m_field3E a straight copy of the local player's
    //  own +0x2E0 word; m_field40 the OR of every *other* player's own
    //  +0x2E0 word whose bit is set in that same local-player mask.  What
    //  the three actually gate elsewhere is not established by this call
    //  tree.
    unsigned short   m_field3C;        // +0x3C
    unsigned short   m_field3E;        // +0x3E
    unsigned short   m_field40;        // +0x40

    //  0x6F408070 - latch m_field3C/3E/40 above off the local player's own
    //  world-visibility record.  `retn 0`.  See jassrefreshworldmasks.cpp.
    void RefreshPlayerWorldVisibilityMasks();

    //  0x6F016CD0 - SetPlayerAlliance's own tail call after
    //  RefreshPlayerWorldVisibilityMasks above (a genuine `jmp`, not a
    //  `call`+`ret`, in the shipped code).  Reaches at least four more
    //  un-dumped callees of its own, one of them through a second tail-jmp
    //  into a chunk (loc_6F0159A0) whose own stack-cleanup convention does
    //  not parse cleanly from a static disassembly alone (a `push ecx` with
    //  no matching pop ahead of its own bare `retn`) - kept a naked
    //  redirect rather than risk a wrong `retn` count on a live hook.
    //  `retn 0`, `this` in ecx.
    void TailCleanup();
};

#endif
