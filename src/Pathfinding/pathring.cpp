//============================================================================
//  0x6F4A3060 and 0x6F4A30D0 / 0x6F4A3280 / 0x6F4A3420 / 0x6F4A35C0 -
//  one lap around a cell rectangle.
//
//  The four walkers are the same four-sided lap started on four different
//  sides, in the same rotational order every time:
//
//        top row, right to left   ->   left column, top to bottom
//              ^                              |
//              |                              v
//        right column, bottom to top  <-  bottom row, left to right
//
//  so ScanRingFromTop runs top/left/bottom/right, ScanRingFromLeft runs
//  left/bottom/right/top, and so on.  Each side stops one cell short of the
//  corner the next side starts on, so no cell is visited twice, and each
//  writes the cell it is looking at into *pt as it goes - the caller reads
//  the accepted cell straight back out of it.
//
//  EvalCandidate is defined in this same translation unit deliberately.  The
//  shipped code has it inlined into three of the four sides of every walker
//  and left as a call on the fourth, which is exactly what an inliner
//  running out of budget part-way through a function does; giving MSVC the
//  same choice is the only way to reproduce that split, and putting the
//  definition anywhere else would force all four to be calls.
//============================================================================
#include "pathfind.h"

//----------------------------------------------------------------------------
//  0x6F4A3060 - test one cell's footprint and, if the caller supplied one,
//  run its filter on the cell converted to a grid position.
//----------------------------------------------------------------------------
int CPathSearch::EvalCandidate(const SCellPoint* pt, const int* mask,
                               unsigned short size, PathFilterFn filter,
                               void* context)
{
    if (!TestFootprint(pt, mask, size))
        return 0;

    if (filter != 0)
    {
        //  The pair the filter is handed: x then y, adjacent, and aligned
        //  to eight.  The alignment is not decoration - it is what makes
        //  every function this body is inlined into build the aligned
        //  `push ebp / mov ebp,esp / and esp,-8` frame the shipped ring
        //  walkers have, and with it their whole argument addressing.  See
        //  docs/msvc-vc8-idioms.md, "An 8-byte-aligned local you can only
        //  see in the caller".
        //
        //  Written as a two-element array rather than two locals so that
        //  the alignment cannot separate them: aligning the first of two
        //  scalars moves it and leaves the second where it was.
        __declspec(align(8)) CFloat pos[2] = { CFloatFromInt(pt->m_x),
                                               CFloatFromInt(pt->m_y) };
        if (!filter(pos, context))
            return 0;
    }
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4A30D0 - top, left, bottom, right.
//----------------------------------------------------------------------------
int CPathSearch::ScanRingFromTop(const SCellBox* box, SCellPoint* pt,
                                 const int* mask, unsigned short size,
                                 PathFilterFn filter, void* context)
{
    *pt = SCellPoint(box->m_x1 - 2, box->m_y1 - 1);
    while (pt->m_x >= box->m_x0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x - 1;
    }

    *pt = SCellPoint(box->m_x0, box->m_y1 - 2);
    while (pt->m_y >= box->m_y0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y - 1;
    }

    *pt = SCellPoint(box->m_x0 + 1, box->m_y0);
    while (pt->m_x < box->m_x1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x + 1;
    }

    *pt = SCellPoint(box->m_x1 - 1, box->m_y0 + 1);
    while (pt->m_y < box->m_y1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y + 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4A3280 - left, bottom, right, top.
//----------------------------------------------------------------------------
int CPathSearch::ScanRingFromLeft(const SCellBox* box, SCellPoint* pt,
                                  const int* mask, unsigned short size,
                                  PathFilterFn filter, void* context)
{
    *pt = SCellPoint(box->m_x0, box->m_y1 - 2);
    while (pt->m_y >= box->m_y0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y - 1;
    }

    *pt = SCellPoint(box->m_x0 + 1, box->m_y0);
    while (pt->m_x < box->m_x1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x + 1;
    }

    *pt = SCellPoint(box->m_x1 - 1, box->m_y0 + 1);
    while (pt->m_y < box->m_y1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y + 1;
    }

    *pt = SCellPoint(box->m_x1 - 2, box->m_y1 - 1);
    while (pt->m_x >= box->m_x0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x - 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4A3420 - bottom, right, top, left.
//----------------------------------------------------------------------------
int CPathSearch::ScanRingFromBottom(const SCellBox* box, SCellPoint* pt,
                                    const int* mask, unsigned short size,
                                    PathFilterFn filter, void* context)
{
    *pt = SCellPoint(box->m_x0 + 1, box->m_y0);
    while (pt->m_x < box->m_x1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x + 1;
    }

    *pt = SCellPoint(box->m_x1 - 1, box->m_y0 + 1);
    while (pt->m_y < box->m_y1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y + 1;
    }

    *pt = SCellPoint(box->m_x1 - 2, box->m_y1 - 1);
    while (pt->m_x >= box->m_x0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x - 1;
    }

    *pt = SCellPoint(box->m_x0, box->m_y1 - 2);
    while (pt->m_y >= box->m_y0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y - 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4A35C0 - right, top, left, bottom.
//----------------------------------------------------------------------------
int CPathSearch::ScanRingFromRight(const SCellBox* box, SCellPoint* pt,
                                   const int* mask, unsigned short size,
                                   PathFilterFn filter, void* context)
{
    *pt = SCellPoint(box->m_x1 - 1, box->m_y0 + 1);
    while (pt->m_y < box->m_y1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y + 1;
    }

    *pt = SCellPoint(box->m_x1 - 2, box->m_y1 - 1);
    while (pt->m_x >= box->m_x0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x - 1;
    }

    *pt = SCellPoint(box->m_x0, box->m_y1 - 2);
    while (pt->m_y >= box->m_y0)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_y = pt->m_y - 1;
    }

    *pt = SCellPoint(box->m_x0 + 1, box->m_y0);
    while (pt->m_x < box->m_x1)
    {
        if (EvalCandidate(pt, mask, size, filter, context))
            return 1;
        pt->m_x = pt->m_x + 1;
    }

    return 0;
}
