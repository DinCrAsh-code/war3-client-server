//============================================================================
//  0x6F2ABF20 - convert a world position to its grid cell and ask the row
//  table what is there.
//
//  Its own translation unit: CWidget::RefreshPlacement (0x6F2ACF90,
//  widgetrefreshplacement.cpp) calls it for real.
//
//  Identical world-to-grid conversion to the one CWidget::MoveForward
//  already does inline (widget.cpp): subtract the map origin, then
//  WorldToGrid - which is not a divide but an exponent subtraction with an
//  underflow flush, see game.h - then truncate to an integer.  Both axes,
//  then RowMaxWithTerrainType with a null `foundOut`.
//
//  `x` arrives in ecx and `y` in edx, and the result is discarded: the call
//  is made for the row table's own side effect, not for the value.
//============================================================================
#include "game.h"
#include "rowrangetable.h"

void __fastcall ProbeWidgetTerrainCell(const CFloat* worldX,
                                       const CFloat* worldY)
{
    int gx = CFloatToInt(WorldToGrid(*worldX - g_pGameData->m_originX));
    int gy = CFloatToInt(WorldToGrid(*worldY - g_pGameData->m_originY));
    RowMaxWithTerrainType(gx, gy, 0);
}
