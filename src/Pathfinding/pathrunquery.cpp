//============================================================================
//  0x6F49B9C0 - CPathClient::RunPathQuery: one whole query pass.
//
//  Rate-limited (movement type 1) against the global tick, then: release any
//  lane a previous pass left held, take a lane and join it, pin the mover's
//  own registration and the region node it is aiming at so nothing moves
//  them while the query runs, run the query on the shared CPathSearch, and
//  rewind the second point cache's cursor over whatever it produced.
//
//  The tail is the interesting part.  The query's own point count is added
//  to the lane's pending total, and a count below 0x20 clears the recompute
//  stamp so the next tick is allowed to try again immediately (a short
//  result means the search ran out of frontier, not out of budget).  Then
//  the first point produced is compared against the caller's destination:
//  if they are the same point the search arrived, and the "target moved"
//  flag is cleared; otherwise that point is scaled from cells into world
//  units and republished as this client's own target, with the flag set.
//
//  Its own translation unit: every call it makes - RateLimit, ReleaseLane,
//  Lane, SPathLane::Join, CTimeSync::GetPathOwner, ClassifySlopeAxis,
//  PinRegistration, RunQuery, RewindCursor, PathOwnerSingleton,
//  ScaleGridVec2 - is a real out-of-line call in the shipped code.
//============================================================================
#include "pathmove.h"
#include "pathfind.h"
#include "pathastar.h"

int CPathClient::RunPathQuery(void* context, const CGridVec2* dest, int budget)
{
    CTimeSync* sync = g_pTimeSync;

    unsigned int age;
    if (RateLimit(1, sync->m_tick, &age) == 0)
        return 0;

    if ((m_flags & kPathFlagSearched) != 0)
        ReleaseLane();

    SPathLane* lane = Lane();
    if (lane->Join(this) == 0)
    {
        //  Stores Join's own zero rather than a fresh literal - the shipped
        //  code reuses the register the test just read.
        m_stamp[1] = 0;
        return 0;
    }

    CPathSearch* owner = sync->GetPathOwner();

    //  Only the low word of the size class is used, and it is halved: the
    //  classifier answers in half-cell steps.
    unsigned int sizeClass = (unsigned int)(unsigned short)ClassifySlopeAxis(&m_radius) >> 1;

    if (m_ignore != 0)
        sync->PinRegistration((const SCellBox*)((char*)m_ignore + 0x1C), 1);
    if (m_targetNode != 0)
        sync->PinRegistration((const SCellBox*)((char*)m_targetNode + 0x1C), 1);

    //  The owner really is the per-cell A* context (pathastar.h): its
    //  0x6F49A800 is CPathAStar::RunSearch, reconstructed there.  Two
    //  sessions arrived at this object from opposite ends and named its
    //  fields differently - this file's `m_extraPoints` at +0x9C is that
    //  file's `m_iterations`, and the quadrant below is the shift that
    //  picks one of the four two-bit passability lanes out of a cell's
    //  flag word.  Calling the real reconstruction rather than a second
    //  naked thunk to the same address is what keeps one definition of
    //  one shipped function.
    ((CPathAStar*)owner)->RunSearch((int)(((m_flags >> 30) & 3) * 2),
                                    &m_cache[1], (const CGridVec2*)context,
                                    dest, m_laneKey, sizeClass, budget);

    RewindCursor(1);

    //  Read once into a local, modified, stored once - not two in-place
    //  read-modify-writes on the field.  The shipped code loads m_flags
    //  into a register before the test and stores it back after both arms.
    //  The test itself is `> 0` on the unsigned field, which is what makes
    //  it a `jbe` rather than a `je`.
    unsigned int flags = m_flags;
    if (owner->m_extraPoints > 0)
        flags |= 0x01000000;
    else
        flags &= ~0x01000000u;
    m_flags = flags;

    unsigned int produced = owner->m_queryCount;
    lane->m_pending += produced;
    if (produced < 0x20)
        m_stamp[1] = 0;

    if (m_ignore != 0)
        sync->PinRegistration((const SCellBox*)((char*)m_ignore + 0x1C), 0);
    if (m_targetNode != 0)
        sync->PinRegistration((const SCellBox*)((char*)m_targetNode + 0x1C), 0);

    //  Compared through the stored bit patterns, not through a by-value
    //  CFloat helper: taking a CFloat by value copies it through a stack
    //  slot first, where the shipped code `fld`s both operands straight out
    //  of the two objects.
    //  The republish path is the fall-through and the "already there" path
    //  is the branch, so the republish is the return written first - and
    //  the test is the `!=` disjunction, not the negation of an `==`
    //  conjunction, which is what puts the first comparison's `jp` on the
    //  arm that skips the second (docs/msvc-vc8-idioms.md).
    const CGridVec2* first = m_cache[1].Points();
    if (*(const float*)&dest->m_x.m_bits != *(const float*)&first->m_x.m_bits ||
        *(const float*)&dest->m_y.m_bits != *(const float*)&first->m_y.m_bits)
    {
        CFloat cellSize(PathOwnerSingleton()->m_grid->m_cellSize.m_bits);
        m_target = ScaleGridVec2(*m_cache[1].Points(), cellSize);
        m_flags |= 0x20000000;
        return 1;
    }

    m_flags &= ~0x20000000u;
    return 1;
}
