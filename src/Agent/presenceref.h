//============================================================================
//  CPresenceRef - `??_7CPresenceRef@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F876D10.json).  Sits between TRefCnt and
//  CAgentField in the real shipped hierarchy that backs the whole
//  Position/SmartPosition/ProjectilePosition family
//  (docs/targets/Position-family.md) - confirmed directly from IDA's own
//  RTTI base-class list for `SmartPosition`:
//
//      public class SmartPosition : public class Position :
//        public class CAgentField : public class CPresenceRef :
//          public class TRefCnt
//
//  `PositionTrack` (Pathfinding/positiontrack.h) was originally written
//  `: public TRefCnt` directly, before this chain was known - the four
//  functions it attributed to itself at slots 2-5 are real, byte-identical
//  reconstructions, they were just filed one level too high (they are
//  `CAgentField`'s own virtuals, inherited unmodified by every class below
//  it, not `Position`'s). See Agent/agentfield.h for that correction; this
//  file only carries what belongs to *this* link of the chain.
//
//  Only slot 0 (TRefCnt::ReleaseSelf, inherited unmodified) is known.
//  Slots 1 and 2 are CPresenceRef's own - both now have real, byte-
//  identical bodies (agentfield-presenceref follow-up pass; see
//  presenceref_deleteself.cpp). Slot 1 is named `DeleteSelf` because its
//  role matches the pattern every other level of this chain repeats
//  (TRefCnt, CPresenceRef, CAgentField, Position, SmartPosition/
//  ProjectilePosition each carry their own slot-1 override).
//
//  **Slot 2 must be named `Detach`, not left generic - this is load-
//  bearing, not a style choice.** CAgentField's own real, already-
//  reconstructed vtable (agent_worktrees/classes/0x6F876D48.json) has only
//  6 slots, with slot 2 landing on 0x6F478920 (CAgentField::Detach,
//  agentfield.h) directly - there is no separate slot anywhere in
//  CAgentField's compiled vtable for a same-named-differently "Method_0x08"
//  to also occupy. C++ only reuses a vtable slot across a base/derived pair
//  when the *derived* virtual has the identical name (and signature) as the
//  base one; a different name gets its own new slot instead, and appending
//  one there would make CAgentField compile to 7 slots instead of the real
//  6, which check_vtables.py exists specifically to catch (see its own
//  header). So CPresenceRef's own slot 2 has to be spelled `Detach` too,
//  for `CAgentField::Detach` to land in the same, correct index as an
//  actual override rather than a sibling addition. Its body (now real, see
//  presenceref_deleteself.cpp) is exactly this base clear: `or eax,-1 /
//  mov [ecx+8],eax / mov [ecx+0Ch],eax / retn` sets offsets +8/+0xC to -1
//  with no stack cleanup and no LookupHandle/AgentSlot5CGate notification
//  step at all - the simpler base version that `CAgentField::Detach`
//  (agentfield.h) extends with that notification on top of the identical
//  `m_typeTag = m_handle = -1` tail - which is why `m_handle`/`m_typeTag`
//  are declared *here*, on `CPresenceRef`, not one level down on
//  `CAgentField` (which adds no fields of its own, only vtable slots).
//============================================================================
#ifndef PRESENCEREF_H
#define PRESENCEREF_H

#include "refcnt.h"

class CPresenceRef : public TRefCnt
{
public:
    //  slot 1 / +0x04 (0x6F02F010) - real, byte-identical in shape to
    //  every other slot-1 override in this chain (reset the vtable to
    //  TRefCnt's own, conditionally SMemFree("delete")) - see
    //  Pathfinding/positiontrackdeleteself.cpp for the proven source shape
    //  this reuses. See presenceref_deleteself.cpp.
    virtual CPresenceRef* DeleteSelf(int flags);

    //  slot 2 / +0x08 (0x6F021900) - real. Must be named `Detach` - see
    //  this file's own header comment for why. Overridden by
    //  CAgentField::Detach (agentfield.h, already reconstructed, real).
    //  See presenceref_deleteself.cpp.
    virtual void Detach();

    unsigned int m_handle;    // +0x08 - table id, -1 when unbound
    unsigned int m_typeTag;   // +0x0C - -1 when unbound
};

#endif
