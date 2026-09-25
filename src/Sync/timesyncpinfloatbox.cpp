//============================================================================
//  0x6F4883D0 - CTimeSync::PinFloatBox.  See game.h.
//
//  Own translation unit for the same reason timesyncpinregistration.cpp is:
//  both of its calls (the cell-box conversion and PinRegistration itself)
//  have to stay real calls, and PinRegistration is the one it would
//  otherwise inline.
//
//  The conversion's `this` is the grid hanging off the path search
//  (m_pathSearch->m_grid), not this object - CCellGrid::FloatBoxToCellBox
//  never reads it in this build (boxmath.cpp), but the shipped call site
//  still loads it, so the source names it.
//============================================================================
#include "game.h"
#include "pathfind.h"
#include "spatialgrid.h"

void CTimeSync::PinFloatBox(const SCellBoxF* box, int pin)
{
    if (box == 0)
    {
        PinRegistration(0, pin);
        return;
    }

    SCellBox cells = m_pathSearch->m_grid->FloatBoxToCellBox(*box);
    PinRegistration(&cells, pin);
}
