//============================================================================
//  0x6F4A3E80 - CPathSearch::FindFreePointNearby: the random-jitter search
//  CWidget::MoveForward's "no rectangle" branch runs instead of the ring
//  walker (see FindFreeWorldPointNearby, pathworldpoint.cpp, and the root's
//  own pseudocode in docs/targets/CWidget__MoveForward.md).
//
//  Same SPathSearchSave/__finally re-entrancy guard as FindFreePoint
//  (pathsearch.cpp) wraps the whole body, and the same
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce at
//  the repo's fixed /GS- /EHs-c- - see docs/msvc-vc8-idioms.md, "An
//  __except_handler4 frame this toolchain cannot reproduce".
//
//  Up to `rings` times: floor-and-clamp the candidate onto a half-cell grid
//  (FloorSplitClampVec against a {0.5, 0.5} pseudo-radius - this is not the
//  caller's own radius, the shipped code always uses the constant half),
//  test the footprint there, and on rejection nudge the candidate by a
//  fresh random draw per axis - RandomCFloat(width, state) where width is
//  jitterBox's own extent on that axis (m_x1-m_x0, m_y1-m_y0) and state is
//  *g_pTimeSync* reinterpreted as an SHashState* (its own first two fields
//  are reserved/unnamed in game.h's CTimeSync, and this is the only place
//  in the call tree that reads them - see hashstate.h).  The draw is a pure
//  [0, width) offset added straight onto the position, not centred on it:
//  a candidate that keeps failing drifts monotonically toward the box's
//  high corner as rings tick down, which is the shipped algorithm, not an
//  approximation of one.
//============================================================================
#include "pathfind.h"
#include "boxmath.h"
#include "hashstate.h"

extern const CFloat g_CFloatHalf;       // dword_6FAAE4D0

int __fastcall ClassifySlopeAxis(const CFloat* value);
void __fastcall FloorSplitClampVec(CGridVec2* pos, const CGridVec2* radius);
void __stdcall RandomCFloat(CFloat* dest, const CFloat* factor,
                            SHashState* state);

int CPathSearch::FindFreePointNearby(CGridVec2* pos, const SCellBoxF* jitterBox,
                                     const CFloat* radius, const int* mask,
                                     int rings, PathFilterFn filter,
                                     void* context)
{
    SPathSearchSave save;
    save.m_search = this;
    save.m_saved = m_searching;
    m_searching = 1;

    __try
    {
        unsigned short size = (unsigned short)ClassifySlopeAxis(radius);

        while (rings != 0)
        {
            rings -= 1;

            CGridVec2 half(g_CFloatHalf, g_CFloatHalf);
            FloorSplitClampVec(pos, &half);

            if (TestFootprintAt(&pos->m_x, mask, size) &&
                (filter == 0 || filter(&pos->m_x, context)))
            {
                return 1;
            }

            CFloat rangeX = jitterBox->m_x1 - jitterBox->m_x0;
            CFloat jitterX;
            RandomCFloat(&jitterX, &rangeX,
                        reinterpret_cast<SHashState*>(g_pTimeSync));
            pos->m_x = pos->m_x + jitterX;

            CFloat rangeY = jitterBox->m_y1 - jitterBox->m_y0;
            CFloat jitterY;
            RandomCFloat(&jitterY, &rangeY,
                        reinterpret_cast<SHashState*>(g_pTimeSync));
            pos->m_y = pos->m_y + jitterY;
        }

        return 0;
    }
    __finally
    {
        RestoreSearchFlag(&save);
    }
}
