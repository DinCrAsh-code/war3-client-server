//============================================================================
//  0x6F00F600 - GetGridCellScalar: stage a cell coordinate pair and forward
//  to the Storm terrain singleton's own per-cell scalar table.
//
//  `__fastcall` with the out pointer in ecx and the row coordinate in edx:
//  the shipped call site (CUnit::RefreshWidgetPlacement,
//  unit_widgetplacement.cpp) stages exactly that shape before the call, not
//  a thiscall member of anything.
//
//  Own translation unit, the same rule 0x6F00BF60/0x6F00F7D0
//  (stormsingleton_terrain.cpp) already follows for the same reason: it is
//  a real, separately addressed function in the dump, and folding it into
//  its one caller's TU lets /O2 inline it away, which loses the `call`
//  the shipped code has.
//============================================================================
#include "stormsingletona.h"
#include "CFloat.h"

//  0x6F01F5A0 - lazysingleton.cpp.
SStormSingletonA* __fastcall GetStormSingletonA();

CFloat* __fastcall GetGridCellScalar(CFloat* out, int rowCoord, int colCoord)
{
    GetStormSingletonA()->QueryCellScalarAt(out, rowCoord, colCoord);
    return out;
}
