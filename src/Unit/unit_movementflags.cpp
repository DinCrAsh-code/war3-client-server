//============================================================================
//  Five small SUnitMovement (CUnit+0x164) setters, all reached only from
//  CUnit's per-template initialiser (0x6F2A0E30, still Method_0x1AC / slot
//  107) - see docs/targets/CUnit__vtable.md for the closure this belongs
//  to.  (A sixth, SetPathClientFlagBit23 at 0x6F474C60, was reached from a
//  different worklist slice of the same slot 107 closure; the file grew
//  to five with EnsurePathClientAndSetFlagBit23 at 0x6F474C10, its
//  immediate address neighbour and near-duplicate, added from a later
//  session's own depth 1-2 slice of the same closure.)
//
//  All four resolve `this` the same way every SUnitMovement/SUnitTrackedRef
//  method does - LookupHandle(m_handle, m_handleType) - but they disagree
//  about what the result *is*:
//
//    * SetPathFlagDefault/SetPathFlagLowNibble call straight through into
//      Position::StoreTopByteBits (0x6F474980, position_flagbyte.cpp) with
//      `this` still the SUnitMovement pointer, reinterpreted as `Position*`
//      for the one call.  That is not a type error in the shipped code:
//      Position keeps its own handle pair at the identical +0x08/+0x0C
//      offset SUnitMovement does (position_flagbyte.cpp's own header says
//      so), so the *same* subroutine works unmodified against either
//      object - MSVC never distinguishes them, it only ever sees `ecx`;
//    * SetLaneReleaseFlag resolves the handle to the owning CUnit itself
//      (pathmove.h's own note - "the movement client itself, cached at
//      unit+0xA8" - is exactly this: LookupHandle(m_handle, m_handleType)
//      on the movement sub-object's own pair answers the CUnit that
//      embeds it) and reaches its CPathClient through CUnit::m_pRefA8.
//============================================================================
#include "unit.h"
#include "positiontrack.h"
#include "pathmove.h"
#include "game.h"

//  Agent/cunit_agent4_unitpathclientprep.cpp - already reconstructed.
extern void __fastcall PrepareUnitPathClient(void* unit, void* unusedEdx, int forceRebuild);

//----------------------------------------------------------------------------
//  0x6F474A00 - stamp the pathing record's flag byte with its startup
//  default: value 0x10, mask 0x70 (clear bits 4-6, set bit 4).  No stack
//  args; `this` is `&m_movement` reinterpreted as `Position*` for the one
//  call, per the file header.
//----------------------------------------------------------------------------
void SUnitMovement::SetPathFlagDefault()
{
    ((PositionTrack*)this)->StoreTopByteBits(0x10, 0x70);
}

//----------------------------------------------------------------------------
//  0x6F4749C0 - fold the incoming byte's own low nibble into the flag
//  byte's low nibble, mask 0x0F.  `movzx`/`and eax, 0FFFFFF0Fh` on a
//  zero-extended byte is exactly `value & 0x0F` with the top 24 bits
//  already zero from the `movzx` - no separate mask needed on this side.
//----------------------------------------------------------------------------
void SUnitMovement::SetPathFlagLowNibble(unsigned char value)
{
    ((PositionTrack*)this)->StoreTopByteBits((unsigned char)(value & 0x0F), 0x0F);
}

//----------------------------------------------------------------------------
//  0x6F476200 - reset both point caches and the search state, drop the
//  formation lane, clear the "moved"/"searched" flags (ResetSearch(-1, 1,
//  0, 1)), then set or clear the CPathClient's own bit 0x200000 from the
//  one stack argument.  `this` really is the CPathClient here, not
//  SUnitMovement recast - `LookupHandle(m_handle, m_handleType)` on the
//  movement sub-object's own pair resolves to the owning CUnit
//  (pathmove.h's "cached at unit+0xA8" note), and CUnit::m_pRefA8 is that
//  CPathClient.
//----------------------------------------------------------------------------
void SUnitMovement::SetLaneReleaseFlag(int on)
{
    CUnit* unit = (CUnit*)LookupHandle(m_handle, m_handleType);
    CPathClient* client = (CPathClient*)unit->m_pRefA8;

    client->ResetSearch(-1, 1, 0, 1);

    //  Read the flags word once, ahead of the branch, and store back once
    //  on whichever arm runs - not two independent read-modify-write
    //  statements.  That is what makes the shipped code load [esi+88h]
    //  between the `cmp` and the `je` rather than inside each arm.
    unsigned int flags = client->m_flags;
    if (on != 0)
        client->m_flags = flags | 0x200000;
    else
        client->m_flags = flags & ~0x200000;
}

//----------------------------------------------------------------------------
//  0x6F474A60 - resolve to the owning unit (same LookupHandle(m_handle,
//  m_handleType) as every method above) and classify its own +0x90 "radius"
//  CFloat the same way CPathClient::m_radius is classified elsewhere
//  (pathrunquery.cpp et al.) - a size-class lookup for the unit's own
//  footprint.  `retn 0`, no stack args, plain `int` return (the shipped
//  `movzx eax, ax` widens ClassifySlopeAxis's own 16-bit answer).  The
//  shipped code copies the field into a local before taking its address
//  (rather than passing `&unit->m_radius90` straight through) - matching
//  that copy-through-local shape is what gets this to EXACT; the direct
//  address spelling scores 0.667 with the same value passed either way.
//----------------------------------------------------------------------------
int SUnitMovement::GetFootprintSizeClass()
{
    CUnit* unit = (CUnit*)LookupHandle(m_handle, m_handleType);
    CFloat radius = unit->m_radius90;
    return (unsigned short)ClassifySlopeAxis(&radius);
}

//----------------------------------------------------------------------------
//  0x6F474C60 - same resolve-to-owning-unit shape, then reaches the unit's
//  CPathClient (CUnit::m_pRefA8, same as SetLaneReleaseFlag above) and
//  drops its current lane through CPathClient::ReleaseLane (0x6F49B5A0,
//  pathwaypointmisc.cpp) before setting or clearing one bit of its own
//  m_flags.  Named for the bit position rather than a guessed purpose -
//  the same "SetFlagBit8" convention CUnit's own slot 42 uses - since
//  nothing in this call tree's own closure resolves what bit 0x800000
//  of CPathClient::m_flags means beyond "cleared whenever the lane is
//  dropped for real".  Same "read once ahead of the branch, store back
//  once per arm" shape SetLaneReleaseFlag already documents.
//----------------------------------------------------------------------------
void SUnitMovement::SetPathClientFlagBit23(int on)
{
    CUnit* unit = (CUnit*)LookupHandle(m_handle, m_handleType);
    CPathClient* client = (CPathClient*)unit->m_pRefA8;

    client->ReleaseLane();

    unsigned int flags = client->m_flags;
    if (on != 0)
        client->m_flags = flags | 0x800000;
    else
        client->m_flags = flags & ~0x800000;
}

//----------------------------------------------------------------------------
//  0x6F474C10 - reached from CUnit's own slot 107 closure (BFS depth 1-2),
//  not the batch-24 worklist SetPathClientFlagBit23 above comes from, but
//  the identical shape once resolved: same LookupHandle(m_handle,
//  m_handleType), same CUnit::m_pRefA8, same ReleaseLane, same
//  set-or-clear of CPathClient::m_flags bit 0x800000.  The one real
//  difference is the extra step first - PrepareUnitPathClient
//  (Agent/cunit_agent4_unitpathclientprep.cpp, already reconstructed),
//  which lazily builds the CPathClient if the unit does not have one yet,
//  rather than assuming - as SetPathClientFlagBit23 does - that a caller
//  already guaranteed one exists.  That extra call is what keeps this a
//  second, real function rather than a duplicate of the one above.
//----------------------------------------------------------------------------
void SUnitMovement::EnsurePathClientAndSetFlagBit23(int on)
{
    CUnit* unit = (CUnit*)LookupHandle(m_handle, m_handleType);

    PrepareUnitPathClient(unit, 0, 1);

    CPathClient* client = (CPathClient*)unit->m_pRefA8;
    client->ReleaseLane();

    unsigned int flags = client->m_flags;
    if (on != 0)
        client->m_flags = flags | 0x800000;
    else
        client->m_flags = flags & ~0x800000;
}
