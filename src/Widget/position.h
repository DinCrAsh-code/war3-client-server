//============================================================================
//  Position: the third base CItem__Constructor (0x6F2B7520) builds, right
//  after FloatMiniB and right before the two embedded CAgentTimer members.
//
//  **This is the same shipped class as Pathfinding/positiontrack.h's own
//  `PositionTrack` - not a different, coincidentally-same-named one.** An
//  earlier session (before that class's own vtable/RTTI chain was known)
//  modelled this class as deriving from an invented placeholder,
//  `PositionVtableBase` - a bare 2-method stub with guessed method names
//  and no refcount field, chosen because nothing in *this* call tree alone
//  proved a deeper hierarchy. `PositionTrack` (`Pathfinding/positiontrack.h`)
//  gives the real chain: `TRefCnt -> CPresenceRef -> CAgentField ->
//  Position`, confirmed both by decoding `CItem::CItem()`'s own immediate
//  bytes (the literal address it writes at its own `+0x64` is
//  `0x6F877DDC` - `PositionTrack`'s own vtable, not a different one) and by
//  `StoreTopByteBits` below resolving `LookupHandle(m_reserved6C,
//  m_reserved70)` - those two fields are `PositionTrack`'s own
//  `m_handle`/`m_typeTag` (`CPresenceRef`, `Agent/presenceref.h`) at the
//  identical offset. See `positiontrack.h`'s own header comment for the
//  full writeup.  This class now derives from the real chain instead of
//  the placeholder; `Position::~Position` (`sub_6F03A9E0`, still `EXACT`)
//  is that whole chain's compiler-*implicit* destructor (needed only here,
//  because this is the one owner among ~30 in this family reconstructed as
//  a real, fully-typed C++ sub-object rather than raw offset stores, so it
//  is the one whose SEH frame needs to be able to unwind it).
//
//  Every field below is a plain dword this call tree writes once and never
//  reads back - no name beyond its offset is recoverable from here (per
//  CLAUDE.md's "do not invent fields the call tree does not read").  Three
//  values are not simple 0/-1 sentinels and are worth flagging even though
//  their purpose stays unknown: +0x7C (0x1), +0x98 (0x4, stored as an
//  immediate rather than through a register the other fields share) and
//  the eleven -1 entries clustered from +0x9C-+0xC4, which repeat in
//  threes (-1,-1,0) three times running - consistent with, but not proof
//  of, an array of three identically-shaped sub-records (index/count/flag
//  triples, matching the -1-as-"unbound" sentinel CAgent's own +0x0C/+0x10
//  already use) that nothing in this call tree indexes into individually.
//  None of these are `PositionTrack`'s own fields - they start right where
//  its true 0x10-byte object ends (`CItem`'s own `+0x74`) and are real,
//  CItem-specific state; ~25 already-reconstructed `Item/*.cpp` call sites
//  read/write them by name as `m_position.<field>`, unmoved by this fix.
//
//  Declared in ascending offset order; the shipped code interleaves the
//  actual stores (some land before Position's own vtable store, most
//  after) because none of them alias each other or the vtable pointer, so
//  the scheduler is free to place whichever ones already have their value
//  in a register first - see docs/msvc-vc8-idioms.md, "Independent chains,
//  reordered".
//
//  Forced: with 23 field stores in the initialiser list plus PositionTrack's
//  own 3 (delegated through its base construction, itself `__forceinline`
//  for the same reason), /Ob2's own cost model declines to fold this
//  constructor into CItem::CItem's single call site on its own (measured -
//  it stays a genuine out-of-line `call`, which the dump does not have;
//  the target's own Position field stores are interleaved with
//  CSelectable/FloatMini's, not walled off behind a call, and all land
//  *before* the two CAgentTimer members that follow - which a real
//  out-of-line call, once made, would also block the scheduler from
//  doing).  Nothing else about the body needs forcing - this is the same
//  "loop-free, single call site, MSVC's heuristic just says no" case
//  storm.h's own LinkToHead documents, not a claim that the shipped
//  compiler needed forcing too.
//============================================================================
#ifndef POSITION_H
#define POSITION_H

#include "positiontrack.h"
#include "itemhandleresolve.h"

class Position : public PositionTrack
{
public:
    //  0x6F2B7520's own inlined tail - see item.h/item.cpp.
    __forceinline Position();

    unsigned int m_reserved74;   // +0x74 - 0

    //  +0x78/+0x7C - a nullable handle ("-1" = unset) and a value CItem's
    //  own vtable[0x180] (index 96, 0x6F266C80 - see CSelectable::Reposition,
    //  selectable.cpp) sets to `1` on every call before notifying whatever
    //  +0x78 names (a later session, CItem::StoreAndNotifyHandle, item.cpp)
    //  - the same "nullable handle, -1 = unset" shape as m_placementRef
    //  below, but a plain int rather than a {handle, typeTag} pair.  The
    //  constructor's own initial `1` for +0x7C (already flagged above as
    //  "not a simple 0/-1 sentinel, worth flagging") is exactly the value
    //  that later write always uses, which is why it was never a plain
    //  zero to begin with.
    unsigned int m_notifyHandle; // +0x78 - -1
    unsigned int m_notifyValue;  // +0x7C - 1
    unsigned int m_reserved80;   // +0x80 - 0
    unsigned int m_reserved84;   // +0x84 - 0
    unsigned int m_reserved88;   // +0x88 - 0

    //  +0x8C/+0x90 - a nullable {handle, typeTag} pair, both fields -1 for
    //  "not set" (JASS_SetItemPosition's own root tests this exact way,
    //  `(m_handle & m_typeTag) != -1`, before resolving it through
    //  QueryHandleField0x54 - itemhandleresolve.h/jasssetitemposition_native.cpp).
    SOptionalHandleRef m_placementRef;

    unsigned int m_reserved94;   // +0x94 - 0
    unsigned int m_reserved98;   // +0x98 - 4 (immediate, not shared with a sentinel register)
    unsigned int m_reserved9C;   // +0x9C - -1
    unsigned int m_reservedA0;   // +0xA0 - -1
    unsigned int m_reservedA4;   // +0xA4 - 0
    unsigned int m_reservedA8;   // +0xA8 - -1
    unsigned int m_reservedAC;   // +0xAC - -1
    unsigned int m_reservedB0;   // +0xB0 - 0
    unsigned int m_reservedB4;   // +0xB4 - -1
    unsigned int m_reservedB8;   // +0xB8 - -1
    unsigned int m_reservedBC;   // +0xBC - 0
    unsigned int m_reservedC0;   // +0xC0 - -1
    unsigned int m_reservedC4;   // +0xC4 - -1
    unsigned int m_reservedC8;   // +0xC8 - 0
    unsigned int m_reservedCC;   // +0xCC - 0
};

inline Position::Position()
    : m_reserved74(0), m_notifyHandle((unsigned int)-1), m_notifyValue(1),
      m_reserved80(0), m_reserved84(0), m_reserved88(0),
      m_placementRef((unsigned int)-1, -1), m_reserved94(0),
      m_reserved98(4), m_reserved9C((unsigned int)-1), m_reservedA0((unsigned int)-1),
      m_reservedA4(0), m_reservedA8((unsigned int)-1), m_reservedAC((unsigned int)-1),
      m_reservedB0(0), m_reservedB4((unsigned int)-1), m_reservedB8((unsigned int)-1),
      m_reservedBC(0), m_reservedC0((unsigned int)-1), m_reservedC4((unsigned int)-1),
      m_reservedC8(0), m_reservedCC(0)
{
}

#endif
