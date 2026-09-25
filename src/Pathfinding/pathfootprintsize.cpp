//============================================================================
//  0x6F4A1D70 - CPathSearch::TestFootprint: latch the mask, then test the
//  footprint for this size class.
//
//  The mask arrives as a pointer and is copied into the search's own
//  m_typeMask before the dispatch, which is what makes every TestCell below
//  match against it without carrying it down as an argument.
//
//  The direction is always 0 here - the whole footprint is tested.  The
//  directional forms exist for callers outside this call tree; the constant
//  zero is what the shipped code pushes.
//
//  Its own translation unit: all four size cases are real calls.
//============================================================================
#include "pathfind.h"

int CPathSearch::TestFootprint(const SCellPoint* pt, const int* mask,
                               unsigned short size)
{
    m_typeMask = *mask;

    switch (size)
    {
    case 0:
        return TestFootprint1(pt, 0);
    case 1:
        return TestFootprint2(pt, 0);
    case 2:
        return TestFootprint3(pt, 0);
    case 3:
        return TestFootprint4(pt, 0);
    }
    return 0;
}
