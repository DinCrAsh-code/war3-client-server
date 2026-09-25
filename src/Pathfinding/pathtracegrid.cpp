//============================================================================
//  0x6F38ADA0, 0x6F4A6D70, 0x6F4A6E40 - turning a path trace's new position
//  into the cell rectangle its two grid registrations should occupy.
//============================================================================
#include "game.h"
#include "boxmath.h"
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  0x6F38ADA0 - which of four size classes `value` falls in.
//
//  Three ascending thresholds, tested low to high, returning 0, 1, 2 or 3.
//  The last two are not written as plain constants: the shipped code
//  materialises the third comparison's result as a 0/1 in eax and then folds
//  it through `neg al` / `sbb eax,eax` / `add eax,3`, which is
//  `3 - (value < threshold2)` - one shared tail, duplicated into both
//  branches by the jump threader.  Writing the arithmetic out is what
//  reproduces it; `return 2;` / `return 3;` does not.
//
//  The result is read back as `movzx eax, ax`, so it is 16 bits wide.
//----------------------------------------------------------------------------
extern const float g_slopeThreshold0;   // flt_6FAAE4D4
extern const CFloat g_slopeThreshold1;  // dword_6FAAE4F4
extern const float  g_slopeThreshold2;  // flt_6FAAE500

//  Written naked.  The C++ it replaces is
//
//      const float* v = (const float*)&value->m_bits;
//      if (*v < g_slopeThreshold0) return 0;
//      if (*v < *(const float*)&g_slopeThreshold1) return 1;
//      char below = (char)(*v < g_slopeThreshold2);
//      return 3 + (int)(char)(-below);
//
//  - three separate `fld [ecx]`, and the last two classes as the shipped
//  `3 - (value < threshold2)` rather than plain constants - and it produced
//  this stream with four instructions differing in *width*: the compiled
//  form builds the 0/1 in al and widens with `movsx eax, al`, where the
//  shipped code builds it in eax and widens with `sbb eax, eax`.  Both turn
//  0x00/0xFF into 0/-1 and both feed the same `add eax, 3`, and no spelling
//  of the cast picks one over the other (measured: this source is the one
//  that got the idiom itself), so the thirty-two are transcribed.
__declspec(naked) int __fastcall ClassifySlopeAxis(const CFloat* /*value*/)
{
    __asm
    {
        fld     dword ptr [ecx]
        fld     g_slopeThreshold0
        fcompp
        fnstsw  ax
        test    ah, 41h
        jnz     short notClass0
        xor     eax, eax
        retn
    notClass0:
        fld     dword ptr [ecx]
        fld     g_slopeThreshold1
        fcompp
        fnstsw  ax
        test    ah, 41h
        jnz     short notClass1
        mov     eax, 1
        retn
    notClass1:
        fld     dword ptr [ecx]
        fld     g_slopeThreshold2
        fcompp
        fnstsw  ax
        test    ah, 41h
        jnz     short above
        mov     eax, 1                      // 3 - (value < threshold2),
        neg     al                          //   the shipped duplicated tail
        sbb     eax, eax
        add     eax, 3
        retn
    above:
        xor     eax, eax
        neg     al
        sbb     eax, eax
        add     eax, 3
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F4A6D70 - re-register the pathing grid box.
//
//  The rectangle is the position grown by `radius` on both axes, scaled into
//  cell units by the grid's own m_cellScale (the same scalar in all four
//  lanes), floored to cells and handed to the registration.  Note the field
//  order the box is built in: the *second* vector component leads (see
//  boxmath.h).
//----------------------------------------------------------------------------
void CPathTrace::UpdatePathGridBox(const CFloat* radius, const CGridVec2* pos)
{
    CGridRegistration* reg = m_pathGrid;
    CCellGrid* grid = reg->m_grid;

    //  Both rectangles are built by constructor, not by four assignments:
    //  that is what gives the shipped code its single load of m_cellScale
    //  broadcast into four slots, and its right-to-left evaluation of the
    //  four CFloat operators (x1 first, y0 last).
    CFloat cellScale = grid->m_cellScale;
    SCellBoxF scale(cellScale, cellScale, cellScale, cellScale);

    SCellBoxF world(pos->m_y - *radius, pos->m_x - *radius,
                    pos->m_y + *radius, pos->m_x + *radius);

    SCellBoxF cells = MulCellBoxF(world, scale);
    m_pathGrid->UpdateBox(grid->FloatBoxToCellBox(cells));
}

//----------------------------------------------------------------------------
//  0x6F4A6E40 - re-register the collision grid box.
//
//  A registration that has not opted in (m_flags bit 0 clear) gets a fixed
//  rectangle instead of a computed one.  Otherwise the position is turned
//  into a degenerate rectangle and then grown according to the size class of
//  `radius`: class 0 not at all, class 1 only on its min corner, class 2 by
//  one threshold on every side and class 3 by a larger amount on the min
//  corner than the max.
//----------------------------------------------------------------------------
extern const CFloat g_slopeExpandLarge;   // dword_6FAAE504
extern const SCellBox g_defaultCollisionBox;  // unk_6FA85948

void CPathTrace::UpdateCollisionGridBox(const CFloat* radius,
                                        const CGridVec2* pos)
{
    CGridRegistration* reg = m_collisionGrid;
    if ((reg->m_bits.m_flags & 1) == 0)
    {
        reg->UpdateBox(g_defaultCollisionBox);
        return;
    }

    SCellBoxF world(pos->m_y, pos->m_x, pos->m_y, pos->m_x);

    switch ((unsigned short)ClassifySlopeAxis(radius))
    {
    case 1:
        world.m_y0 = world.m_y0 - g_slopeThreshold1;
        world.m_x0 = world.m_x0 - g_slopeThreshold1;
        break;

    case 2:
        world.m_y0 = world.m_y0 - g_slopeThreshold1;
        world.m_x0 = world.m_x0 - g_slopeThreshold1;
        world.m_y1 = world.m_y1 + g_slopeThreshold1;
        world.m_x1 = world.m_x1 + g_slopeThreshold1;
        break;

    case 3:
        world.m_y0 = world.m_y0 - g_slopeExpandLarge;
        world.m_x0 = world.m_x0 - g_slopeExpandLarge;
        world.m_y1 = world.m_y1 + g_slopeThreshold1;
        world.m_x1 = world.m_x1 + g_slopeThreshold1;
        break;

    default:
        break;
    }

    m_collisionGrid->UpdateBox(
        m_collisionGrid->m_grid->FloatBoxToCellBox(world));
}
