//============================================================================
//  0x6F49BD10 - CPathClient::TestSegmentWalkable.
//
//  Given a start point, a per-step delta and a limit, march t = 1, 2, 3 ...
//  along `from + step*t` and ask the shared CPathSearch whether the
//  footprint fits at each *new* cell the march enters.  Cells the march
//  passes through twice in a row are asked once; the direction bits handed
//  to the footprint test are the ones from the previous cell to this one,
//  so each test only re-checks the cells the step brought newly under the
//  footprint (pathfind.h's own note on the direction-aware variants).
//
//  Its own translation unit: the four footprint tests it dispatches to and
//  DirectionBetween are all real out-of-line calls in the shipped code.
//============================================================================
#include "pathmove.h"
#include "pathfind.h"
#include "boxmath.h"

//  dword_6FAAE4F4 - encoded 1.0, both the initial t and the per-step
//  increment.  Already named in funcmap's DATA table and declared with this
//  exact type elsewhere (pathmove.h) - a second spelling would be a second
//  symbol the linker never resolves.

int CPathClient::TestSegmentWalkable(const CGridVec2* from,
                                     const CGridVec2* step, CFloat limit)
{
    //  Dead by the time the optimiser is finished with it - nothing ever
    //  reads this cell - but the two conversions are real out-of-line
    //  calls, so the stores vanish and the calls stay.  The shipped code
    //  makes both, in this order (MSVC evaluates constructor arguments
    //  right to left, so the y conversion runs first).  Kept rather than
    //  deleted because deleting it deletes four instructions the target
    //  has.
    SCellPoint startCell(CFloatToInt(CFloatFloor(from->m_x)),
                         CFloatToInt(CFloatFloor(from->m_y)));

    SCellPoint cell(0, 0);
    CFloat t = g_slopeThreshold1;

    //  `limit` is compared through a volatile-qualified view of its own
    //  argument slot.  Read by name - or through a plain `const float*` -
    //  it is loop-invariant, and this compiler hoists the `fld` out of the
    //  loop and parks the value on the x87 stack for the whole march,
    //  which also swaps the two comparison operands, changes the
    //  status-word mask and forces a second copy of the test at the bottom
    //  of the loop.  The shipped code reloads it from `[ebp+arg_8]` on
    //  every iteration.  `volatile` is the spelling that reproduces that;
    //  it is not a behavioural claim - nothing writes `limit`, so a reload
    //  reads the same bits the hoisted value held.
    const volatile float* limitBits = (const volatile float*)&limit;

    while (*(const float*)&t.m_bits < *limitBits)
    {
        SCellPoint prev = cell;

        CFloat scaledY = step->m_y * t;
        CFloat scaledX = step->m_x * t;
        CFloat pointY  = from->m_y + scaledY;
        CFloat pointX  = from->m_x + scaledX;

        cell.m_y = CFloatToInt(CFloatFloor(pointY));
        cell.m_x = CFloatToInt(CFloatFloor(pointX));

        if (cell.m_x != prev.m_x || cell.m_y != prev.m_y)
        {
            int dir = DirectionBetween(&cell.m_x, &prev.m_x);

            int fits;
            switch (g_pathQuality)
            {
            case 0:  fits = g_pPathSearch->TestFootprint1(&cell, dir); break;
            case 1:  fits = g_pPathSearch->TestFootprint2(&cell, dir); break;
            case 2:  fits = g_pPathSearch->TestFootprint3(&cell, dir); break;
            case 3:  fits = g_pPathSearch->TestFootprint4(&cell, dir); break;
            default: return 0;
            }

            if (fits == 0)
                return 0;
        }

        t = t + g_slopeThreshold1;
    }

    return 1;
}
