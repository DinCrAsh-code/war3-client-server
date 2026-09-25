//============================================================================
//  0x6F4A2750 / 0x6F4A2880 / 0x6F4A29C0 / 0x6F4A2B00 - the four size-indexed
//  8-neighbour walks of the fine per-cell A* (pathfinesearch.h).
//
//  One perimeter query per call answers all eight directions: the ring around
//  a `size` footprint is numbered clockwise from its north-west corner -
//  corner, `size` cells of north edge, corner, `size` of east, and so on, for
//  4 * (size + 1) bits - and each direction reads the bits it needs out of
//  that one mask.  A cardinal needs its own edge clear.  A diagonal needs its
//  corner and *both* flanking edges clear, which is the no-cutting-through-a-
//  corner rule and the only reason the diagonal masks are wider than the
//  cardinal ones.
//
//  Every direction the mask rules out is written back as -1 instead of being
//  skipped, so CPathFineSearch::Expand can index the array by direction.
//  Spelled `== 0 ? lookup : -1` rather than `? -1 : lookup` because the
//  shipped code puts the lookup on the fall-through and the -1 out of line;
//  the other way round inverts every one of the eight branches.
//
//  The four are separate functions rather than one taking a size because the
//  masks are compile-time constants in each: the shipped code has them as
//  `test bl,83h` immediates, not as loads from a table.
//
//  Own translation unit: Expand reaches all four with real calls through a
//  jump table.
//============================================================================
#include "pathfinesearch.h"

//----------------------------------------------------------------------------
//  0x6F4A2750 - footprint size 1, 8 perimeter bits.
//----------------------------------------------------------------------------
void CPathFineSearch::Neighbours1(const SCellPoint* at, int* out)
{
    int ring = TestPerimeter(at->m_x, at->m_y, 1, 3);

    out[0] = (ring & 0x83) == 0 ? LookupNode(at->m_x - 1, at->m_y - 1) : -1;
    out[1] = (ring & 0x02) == 0 ? LookupNode(at->m_x,     at->m_y - 1) : -1;
    out[2] = (ring & 0x0E) == 0 ? LookupNode(at->m_x + 1, at->m_y - 1) : -1;
    out[3] = (ring & 0x80) == 0 ? LookupNode(at->m_x - 1, at->m_y) : -1;
    out[4] = (ring & 0x08) == 0 ? LookupNode(at->m_x + 1, at->m_y) : -1;
    out[5] = (ring & 0xE0) == 0 ? LookupNode(at->m_x - 1, at->m_y + 1) : -1;
    out[6] = (ring & 0x20) == 0 ? LookupNode(at->m_x,     at->m_y + 1) : -1;
    out[7] = (ring & 0x38) == 0 ? LookupNode(at->m_x + 1, at->m_y + 1) : -1;
}

//----------------------------------------------------------------------------
//  0x6F4A2880 - footprint size 2, 12 perimeter bits.
//----------------------------------------------------------------------------
void CPathFineSearch::Neighbours2(const SCellPoint* at, int* out)
{
    int ring = TestPerimeter(at->m_x, at->m_y, 2, 4);

    out[0] = (ring & 0x0C07) == 0 ? LookupNode(at->m_x - 1, at->m_y - 1) : -1;
    out[1] = (ring & 0x06) == 0 ? LookupNode(at->m_x,     at->m_y - 1) : -1;
    out[2] = (ring & 0x3E) == 0 ? LookupNode(at->m_x + 1, at->m_y - 1) : -1;
    out[3] = (ring & 0x0C00) == 0 ? LookupNode(at->m_x - 1, at->m_y) : -1;
    out[4] = (ring & 0x30) == 0 ? LookupNode(at->m_x + 1, at->m_y) : -1;
    out[5] = (ring & 0x0F80) == 0 ? LookupNode(at->m_x - 1, at->m_y + 1) : -1;
    out[6] = (ring & 0x0180) == 0 ? LookupNode(at->m_x,     at->m_y + 1) : -1;
    out[7] = (ring & 0x01F0) == 0 ? LookupNode(at->m_x + 1, at->m_y + 1) : -1;
}

//----------------------------------------------------------------------------
//  0x6F4A29C0 - footprint size 3, 16 perimeter bits.
//----------------------------------------------------------------------------
void CPathFineSearch::Neighbours3(const SCellPoint* at, int* out)
{
    int ring = TestPerimeter(at->m_x, at->m_y, 2, 5);

    out[0] = (ring & 0xE00F) == 0 ? LookupNode(at->m_x - 1, at->m_y - 1) : -1;
    out[1] = (ring & 0x0E) == 0 ? LookupNode(at->m_x,     at->m_y - 1) : -1;
    out[2] = (ring & 0xFE) == 0 ? LookupNode(at->m_x + 1, at->m_y - 1) : -1;
    out[3] = (ring & 0xE000) == 0 ? LookupNode(at->m_x - 1, at->m_y) : -1;
    out[4] = (ring & 0xE0) == 0 ? LookupNode(at->m_x + 1, at->m_y) : -1;
    out[5] = (ring & 0xFE00) == 0 ? LookupNode(at->m_x - 1, at->m_y + 1) : -1;
    out[6] = (ring & 0x0E00) == 0 ? LookupNode(at->m_x,     at->m_y + 1) : -1;
    out[7] = (ring & 0x0FE0) == 0 ? LookupNode(at->m_x + 1, at->m_y + 1) : -1;
}

//----------------------------------------------------------------------------
//  0x6F4A2B00 - footprint size 4, 20 perimeter bits.
//----------------------------------------------------------------------------
void CPathFineSearch::Neighbours4(const SCellPoint* at, int* out)
{
    int ring = TestPerimeter(at->m_x, at->m_y, 3, 6);

    out[0] = (ring & 0xF001F) == 0 ? LookupNode(at->m_x - 1, at->m_y - 1) : -1;
    out[1] = (ring & 0x1E) == 0 ? LookupNode(at->m_x,     at->m_y - 1) : -1;
    out[2] = (ring & 0x03FE) == 0 ? LookupNode(at->m_x + 1, at->m_y - 1) : -1;
    out[3] = (ring & 0xF0000) == 0 ? LookupNode(at->m_x - 1, at->m_y) : -1;
    out[4] = (ring & 0x03C0) == 0 ? LookupNode(at->m_x + 1, at->m_y) : -1;
    out[5] = (ring & 0xFF800) == 0 ? LookupNode(at->m_x - 1, at->m_y + 1) : -1;
    out[6] = (ring & 0x7800) == 0 ? LookupNode(at->m_x,     at->m_y + 1) : -1;
    out[7] = (ring & 0x7FC0) == 0 ? LookupNode(at->m_x + 1, at->m_y + 1) : -1;
}

