//============================================================================
//  Clamping a grid position to the map's playable bounds (0x6F469490).
//============================================================================
#ifndef GAMEBOUNDS_H
#define GAMEBOUNDS_H

#include "game.h"
#include "quadrant.h"

//  CGameData carries the map's playable rectangle just past the origin pair
//  game.h already names: the low corner at +0x6C/+0x70 (m_originX/m_originY)
//  and the high corner at +0x74/+0x78.  CGameData's declaration was fixed by
//  an earlier target up to +0x70 and must not grow a second, contradicting
//  tail here, so the high corner is read through raw offsets - see CLAUDE.md,
//  "struct offsets are exact, and only touched members get names".
const int kGameDataBoundHiX = 0x74;
const int kGameDataBoundHiY = 0x78;

//  The inset applied to the high corner on both axes.
extern const CFloat g_boundMargin;      // dword_6FAAE564

//----------------------------------------------------------------------------
//  0x6F469490 - clamp a grid position into
//  [originX, hiX - margin] x [originY, hiY - margin].
//----------------------------------------------------------------------------
void __fastcall ClampToGameBounds(CGridVec2* pos);

#endif
