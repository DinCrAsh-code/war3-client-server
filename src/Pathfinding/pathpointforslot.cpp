//============================================================================
//  0x6F49C5B0 - CPathClient::PointForSlot: the point a waypoint slot means.
//
//  The queued-waypoint branch does NOT interpolate between two adjacent
//  points, which is what an earlier reconstruction here assumed.  It reads
//  ONE point - `m_cache[1].Points()[m_cursor[1]]`, the cache's own live
//  cursor rather than the requested slot - and scales it from grid units
//  into world units by the grid's cell size, exactly as PopWaypoint does a
//  few functions over (`ScaleGridVec2(points[m_cursor[1] - 2], cellSize)`).
//  `slot` only ever selects *which bound is tested*, never which point is
//  returned.
//
//  The bound is `m_cursor[slot+1] < m_cache[slot+1].m_count`, and both
//  indices really do run past the two declared entries - the shipped code
//  addresses `[esi + (slot+1)*4 + 74h]` and `[esi + (slot+1)*20h + 50h]`,
//  which are `m_cursor[k]` and `m_cache[k].m_count` for a k that is not
//  bounded to 0..1 here.  Spelling them as the declared arrays is what
//  makes MSVC emit those two addressing modes; it is deliberate, not an
//  overrun waiting to be "fixed".  0x6F49C8D0 indexes the same two arrays
//  the same way - see its note in pathmove.h.
//
//  Two details the disassembly fixes and that cost score if changed:
//  the two CFloat multiplies happen y-first, x-second, and are then stored
//  x-first, y-second (hence the named locals rather than a CGridVec2
//  temporary); and the fallback's slot test is *unsigned* (`test ecx,ecx`
//  / `ja`), so it reads "slot != 0", not "slot > 0".
//============================================================================
#include "pathmove.h"

CGridVec2* CPathClient::PointForSlot(CGridVec2* out, int slot)
{
    if (slot != 1 && m_cursor[1] != 0)
    {
        unsigned int next = (unsigned int)(slot + 1);
        if (m_cursor[next] < m_cache[next].m_count)
        {
            //  Constructed from the bit pattern, not copy-constructed from
            //  the member: the copy constructor takes a reference, so MSVC
            //  forms the member's address as well as loading its value and
            //  the address is then dead.
            CFloat cellSize(PathOwnerSingleton()->m_grid->m_cellSize.m_bits);
            const CGridVec2& p = m_cache[1].Points()[m_cursor[1]];

            //  y before x - the shipped code calls operator* on the y
            //  component first and keeps its result buffer in edi while it
            //  computes x.
            CFloat y = p.m_y * cellSize;
            CFloat x = p.m_x * cellSize;
            out->m_x = x;
            out->m_y = y;
            return out;
        }
    }

    *out = ((unsigned int)slot > 0) ? m_target : m_position;
    return out;
}
