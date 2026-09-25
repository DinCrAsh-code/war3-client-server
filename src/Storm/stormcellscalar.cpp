//============================================================================
//  0x6F7442F0 - SStormSingletonA::QueryCellScalarAt.  See stormsingletona.h.
//
//  Own translation unit: CUnit::RefreshWidgetPlacement's own GetGridCellScalar
//  (unit_widgetplacement.cpp) is its only caller in this tree.
//============================================================================
#include "stormsingletona.h"
#include "CFloat.h"

extern const CFloat g_CFloatZero;   // 0x6FAAE470

//  Clamp into [0, bound] the same way twice - once per axis, both against
//  this same object's own m_recordStride/m_maxTableY.  A shared helper
//  reproduces the shipped `cmp`/`jbe`/`sar 1Fh`/`not`/`and` idiom for a
//  signed value against an unsigned bound.
static __forceinline int ClampToBound(int value, int bound)
{
    if ((unsigned int)value > (unsigned int)bound)
        value = bound & ~(value >> 31);
    return value;
}

void SStormSingletonA::QueryCellScalarAt(CFloat* out, int rowCoord, int colCoord) const
{
    if (m_cellScalarTableReady == 0)
    {
        *out = g_CFloatZero;
        return;
    }

    //  `/4`, not `>>2`: the shipped code has the full round-toward-zero
    //  division idiom (`cdq`/`and 3`/`add`/`sar 2`), which a raw shift does
    //  not produce - these coordinates can be negative on the interpolated
    //  fallback path (GetCellMidpoint, unit_widgetplacement.cpp).
    //
    //  Register allocation only past this point: the shipped code
    //  schedules the two divisions' shared `cdq`/`and 3`/`add` halves
    //  together before either one's final `sar 2`, and keeps the result in
    //  two registers across both clamps and the final index; this build
    //  reaches the identical instruction set (round-to-zero divide by
    //  four, branchy clamp against m_recordStride-1/m_maxTableY-1,
    //  `stride*col+row` index) with one extra saved register.  See
    //  funcmap.py's BEHAVIOUR entry.
    int row = ClampToBound(rowCoord / 4, m_recordStride - 1);
    int col = ClampToBound(colCoord / 4, m_maxTableY - 1);

    *(int*)out = m_cellScalarTable[m_recordStride * col + row];
}
