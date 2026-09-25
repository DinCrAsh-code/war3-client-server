//============================================================================
//  CAgentCantPathEvent (RTTI-named, `agent_worktrees/classes/0x6F951B00.json`,
//  hierarchy `CEvent, TRefCnt`) - a 2-slot vtable, both slots inherited
//  unchanged in shape from `TRefCnt` (Agent/refcnt.h): `CEvent` itself adds
//  no virtual of its own (Class Informer's own `methods: 2` for this class
//  confirms the count never grows past TRefCnt's own two), and
//  `CAgentCantPathEvent` overrides only the scalar deleting destructor
//  (slot 1, 0x6F4722D0) to restamp `this` straight to `??_7TRefCnt@@6B@`
//  and free it - the exact same "no base teardown, no `this != 0` guard"
//  shape Agent/presence.h's whole `ScalarDeletingDestructorTrivial` family
//  already documents for CPresence's own descendants, just restamping one
//  level further up the chain (to TRefCnt rather than to CPresence) because
//  this class's own base, CEvent, has nothing non-trivial of its own to
//  fold away either. No fields of either CEvent or CAgentCantPathEvent are
//  read or written anywhere in this call tree, so none are modelled here.
//============================================================================
#ifndef AGENTCANTPATHEVENT_H
#define AGENTCANTPATHEVENT_H

#include "refcnt.h"

//  A minimal placeholder for `CEvent` - only known here as "a TRefCnt
//  subclass that adds no vtable slot of its own" (see file header above).
//  Not modelled beyond that; nothing in this call tree needs more.
class CEvent : public TRefCnt
{
};

class CAgentCantPathEvent : public CEvent
{
public:
    //  Slot 0 (0x6F00C060) is TRefCnt::ReleaseSelf, inherited unmodified -
    //  no override here, matching Pathfinding/pathingmapindicator.h's own
    //  precedent for the same inherited-slot-0 shape.

    //  Slot 1 (0x6F4722D0) - scalar deleting destructor.  See this file's
    //  own header comment for why it restamps to TRefCnt's vtable rather
    //  than its own or CEvent's.
    void* ScalarDeletingDestructor(unsigned int flags);
};

#endif
