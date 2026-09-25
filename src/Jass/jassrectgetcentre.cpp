//============================================================================
//  0x6F3B1FB0 - SJassRectCentre::GetCentre.  `retn 4`, `this` being the
//  bounds block at +0x24 (see jassnatives.h).
//
//  Own translation unit in the 0x6F3B1xxx/0x6F3B2xxx rect-geometry module:
//  MoveTo (jassrectmoveto.cpp) issues a genuine `call` to this address, so
//  the two must not share a TU or MSVC folds this body into that call site.
//
//  Same "one word crossed" labelling SJassRect's own comment records: the
//  two centres this writes are averages of the *labelled* min/max pairs,
//  not of the axis the caller's own name for the result implies.
//  `out[0]` gets the average of the labelled Y pair (m_maxY... no -
//  m_maxY/m_minY at +0x0C/+0x04) and `out[1]` the labelled X pair
//  (m_maxX/m_minX at +0x08/+0x00) - which is what
//  JASS_GetRectCenterX (jassnatives_rect.cpp) reading `centre[0]` as its
//  answer is really averaging, the same crossing GetRectMinX/GetRectMaxX
//  already read straight through.
//============================================================================
#include "jassnatives.h"

int* SJassRectCentre::GetCentre(int* out)
{
    //  One chained expression, not a named sum: the shipped code pushes
    //  g_CFloatHalf once, up front, and reuses operator+'s own returned
    //  address directly as operator*'s left operand - a named intermediate
    //  forces a spill/reload of that pointer instead.
    CFloat centreX = (m_maxX + m_minX) * g_CFloatHalf;
    CFloat centreY = (m_maxY + m_minY) * g_CFloatHalf;

    out[0] = centreY.m_bits;
    out[1] = centreX.m_bits;
    return out;
}
