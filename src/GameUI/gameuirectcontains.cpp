//============================================================================
//  0x6F332540 - SScreenRect::Contains.
//
//  Four x87 comparisons against the rectangle's own four floats, all four of
//  which have to pass.  The mask is what picks the comparison: `test ah, 5`
//  / `jp` leaves through the failure exit when C0 and C2 agree, i.e. the
//  test that continues is "the point's component is strictly greater than
//  the bound"; `test ah, 41h` / `jnz` leaves when C0 or C3 is set, i.e. the
//  test that continues is "strictly less".  See docs/msvc-vc8-idioms.md,
//  "A float comparison's status-word mask, decoded".
//
//  The point is named first in every one of the four, which is why its
//  component is the `fld` of each pair and the bound is the second.
//
//  Own translation unit: 0x6F333CB0 calls it for real, and this is a
//  different shipped module.
//============================================================================
#include "gameui.h"

int SScreenRect::Contains(const float* point) const
{
    if (point[0] > m_minY && point[0] < m_maxY
        && point[1] > m_minX && point[1] < m_maxX)
        return 1;

    return 0;
}
