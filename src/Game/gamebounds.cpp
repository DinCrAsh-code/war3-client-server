//============================================================================
//  0x6F469490 - clamp a grid position into the map's playable rectangle.
//
//  Both axes are the same three-way select, and both are written as one
//  conditional expression rather than a pair of `if` statements: the shipped
//  code picks an *address* (the low bound, the high bound or the position
//  itself) through the compare chain and stores through it exactly once,
//  which is what a nested ternary over `const CFloat&` produces and what a
//  pair of assignments does not.  It is also why the unclamped case still
//  performs a store - of the value onto itself.
//============================================================================
#include "gamebounds.h"

void __fastcall ClampToGameBounds(CGridVec2* pos)
{
    CGameData* gd = g_pGameData;
    //  The margin is copied into a local first - the shipped code loads it
    //  once and passes the local's address to both subtractions.
    CFloat margin = g_boundMargin;

    {
    const CFloat& hiX =
        *(const CFloat*)((const char*)gd + kGameDataBoundHiX) - margin;
    const CFloat* px = &gd->m_originX;
    if (!(*(const float*)&pos->m_x < *(const float*)&gd->m_originX))
    {
        px = &hiX;
        if (!(*(const float*)&pos->m_x > *(const float*)&hiX))
            px = &pos->m_x;
    }
    pos->m_x = *px;
    }

    {
    const CFloat& hiY =
        *(const CFloat*)((const char*)gd + kGameDataBoundHiY) - margin;
    //  The y axis returns early out of the low-bound case rather than
    //  falling through a shared store: it is the last statement in the
    //  function, and that is the block order the shipped code has.
    if (*(const float*)&pos->m_y < *(const float*)&gd->m_originY)
    {
        pos->m_y = gd->m_originY;
        return;
    }
    const CFloat* py = &hiY;
    if (!(*(const float*)&pos->m_y > *(const float*)&hiY))
        py = &pos->m_y;
    pos->m_y = *py;
    }
}
