//============================================================================
//  0x6F4A3FF0 - CPathSearch::FindFreePoint: the outward ring search.
//
//  The starting rectangle is tested first, but only as a single point and
//  only when it is degenerate (zero width or zero height on either axis) -
//  a real rectangle goes straight into the ring walk.  From there the
//  rectangle is grown by one cell on every side per ring and one lap is
//  walked, up to `rings` times.
//
//  The whole body runs with m_searching set, which relaxes one of TestCell's
//  rejections, and an `__finally` puts it back however the function leaves -
//  including through an exception thrown by the caller's filter.  The saved
//  value and the search it belongs to are the two-field frame the funclet at
//  0x6F468700 is handed.
//
//  The four ring walkers are dispatched 0, 3, 2, 1 - the block order the
//  shipped code lays them out in - and all four share one test of the
//  result, which is why the answer goes through a variable instead of four
//  separate `if (...) goto`s.
//
//  `wholeCells` picks how the answer is handed back: the cell index itself,
//  or the cell's centre (index + a half).
//============================================================================
#include "pathfind.h"

//  The half a cell added to each axis when the caller wants cell centres.
extern const CFloat g_CFloatHalf;       // dword_6FAAE4D0

int __fastcall ClassifySlopeAxis(const CFloat* value);

int CPathSearch::FindFreePoint(CFloat* out, const SCellBoxF* box,
                               int startSide, const CFloat* radius,
                               const int* mask, int rings,
                               PathFilterFn filter, void* context,
                               int wholeCells)
{
    SPathSearchSave save;
    save.m_search = this;
    save.m_saved = m_searching;
    m_searching = 1;

    __try
    {
        unsigned short size = (unsigned short)ClassifySlopeAxis(radius);

        if (!(*(const float*)&box->m_y0 < *(const float*)&box->m_y1 &&
              *(const float*)&box->m_x0 < *(const float*)&box->m_x1))
        {
            //  The x/y pair.  Note it is *not* spelled with the
            //  __declspec(align(8)) EvalCandidate's pair carries, even
            //  though the shipped frame here is aligned too: MSVC cannot
            //  combine an aligned local with this function's SEH frame
            //  without building a second, ebx-based frame on top of the
            //  ebp one, which costs far more than the alignment buys.
            CFloat pos[2] = { box->m_x0, box->m_y0 };
            if (TestFootprintAt(pos, mask, size) &&
                (filter == 0 || filter(pos, context)))
            {
                out[0] = pos[0];
                out[1] = pos[1];
                return 1;
            }
            rings = rings - 1;
        }

        {
            SCellBox cells = m_grid->FloatBoxToCellBox(*box);
            SCellPoint pt(0, 0);
            //  Hoisted out of the loop: the shipped code leaves the switch
            //  result wherever the call left it and tests that, so an
            //  in-loop initialisation would be two stores an iteration that
            //  the shipped code does not make.  The default case cannot be
            //  reached from any caller in this tree - startSide is always
            //  one of the four sides - and this keeps it defined anyway.
            int found = 0;

            while (rings != 0)
            {
                rings -= 1;
                cells.m_y0 -= 1;
                cells.m_x0 -= 1;
                cells.m_y1 += 1;
                cells.m_x1 += 1;

                switch (startSide)
                {
                case 0:
                    found = ScanRingFromTop(&cells, &pt, mask, size, filter,
                                            context);
                    break;
                case 3:
                    found = ScanRingFromRight(&cells, &pt, mask, size, filter,
                                              context);
                    break;
                case 2:
                    found = ScanRingFromBottom(&cells, &pt, mask, size, filter,
                                               context);
                    break;
                case 1:
                    found = ScanRingFromLeft(&cells, &pt, mask, size, filter,
                                             context);
                    break;
                }

                if (found)
                {
                    if (wholeCells != 0)
                    {
                        out[0] = CFloatFromInt(pt.m_x);
                        out[1] = CFloatFromInt(pt.m_y);
                    }
                    else
                    {
                        //  One expression, so the y half is computed first
                        //  and both stores happen after both additions -
                        //  the shipped order.
                        out[0] = CFloatFromInt(pt.m_x) + g_CFloatHalf;
                        out[1] = CFloatFromInt(pt.m_y) + g_CFloatHalf;
                    }
                    return 1;
                }
            }
        }

        return 0;
    }
    __finally
    {
        RestoreSearchFlag(&save);
    }
}
