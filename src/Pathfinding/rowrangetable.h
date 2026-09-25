//============================================================================
//  0x6F0118C0 - a table of axis-aligned rectangles, each carrying its own
//  flat data array, scanned for every rectangle containing a point and
//  reduced to the largest value found among them.
//
//  What builds this table, and what the values mean, are both outside this
//  call tree; only the shape FindMax() itself reads is named.  The header
//  gap at +0x1C..+0x24 is real - the data pointer sits four dwords past the
//  last bound this function touches, and nothing in this call tree says
//  what fills the gap.
//============================================================================
#ifndef ROWRANGETABLE_H
#define ROWRANGETABLE_H

struct SRowRange
{
    char          m_reserved00[0x0C];
    int           m_yLo;      // +0x0C
    int           m_xLo;      // +0x10
    int           m_yHi;      // +0x14
    int           m_xHi;      // +0x18
    char          m_reserved1C[0x24 - 0x1C];
    int*          m_data;     // +0x24 - row-major, width (m_xHi-m_xLo+1)
};

class CRowRangeTable
{
public:
    //  0x6F0118C0 - scan every rectangle containing (x, y); if at least one
    //  does, write the largest value found among them through `out` (when
    //  non-null) and return 1, otherwise return 0 and leave *out untouched.
    int FindMax(int x, int y, int* out) const;

    char          m_reserved00[4];
    unsigned int  m_count;    // +0x04
    SRowRange*    m_ranges;   // +0x08 - stride sizeof(SRowRange) = 0x2C
};

//----------------------------------------------------------------------------
//  0x6F0128C0 - the larger of a terrain-type nibble and the CRowRangeTable
//  max at (x, y). See rowmaxwithterraintype.cpp.
//----------------------------------------------------------------------------
int __fastcall RowMaxWithTerrainType(int x, int y, int* foundOut);

#endif
