//============================================================================
//  0x6F0128C0 - the larger of the terrain type nibble at (x, y) and the
//  largest CRowRangeTable value covering that point, with an optional
//  "a row range table entry was found" out-flag.
//
//  GetStormSingletonB()'s return doubles as a CRowRangeTable* here - the
//  same 0x10-byte allocation, zeroed the same way at construction
//  (m_f0..m_fC), just read through CRowRangeTable's field roles instead of
//  the four opaque ints lazysingleton.cpp names it with elsewhere in this
//  call tree.
//============================================================================
#include "rowrangetable.h"
#include "stormsingletona.h"

struct SStormSingletonB;
SStormSingletonA* __fastcall GetStormSingletonA();
SStormSingletonB* __fastcall GetStormSingletonB();

int __fastcall RowMaxWithTerrainType(int x, int y, int* foundOut)
{
    if (foundOut)
        *foundOut = 0;

    int terrainType = GetStormSingletonA()->QueryCell(x, y);

    int rowMax;
    if (((CRowRangeTable*)GetStormSingletonB())->FindMax(x, y, &rowMax))
    {
        if (foundOut)
            *foundOut = 1;
        if (rowMax > terrainType)
            return rowMax;
    }
    return terrainType;
}
