//============================================================================
//  0x6F3DB8F0 - SJassRect::SetBounds, the four bounds written in one call.
//
//  `retn 10h`: the block arrives by value, which is why JASS_RectFromLoc
//  carves sixteen bytes off the stack by hand and writes the four words into
//  them rather than pushing four times.
//
//  0x6F3DB9D0 - SJassRect::Resize, and it really is a second function and
//  not this one seen twice: two distinct addresses 0xE0 apart, each reached
//  by its own callers, whose bodies happen to be the same nine instructions.
//  Which one a caller uses splits cleanly along "is the rect new" - the
//  factory native `RectFromLoc` calls SetBounds on the rect it has just
//  made, while the two mutating natives `SetRect` (0x6F3C1710) and
//  `SetRectFromLoc` (0x6F3C17F0) call this one on a rect they resolved from
//  a handle - so they are kept as two names rather than one, and neither
//  call site is free to reach the other's address.
//
//  Own translation unit in the 0x6F3DBxxx module, next door to the '+rct'
//  type-id leaf at 0x6F3DB8A0 (jassagiletypeids.cpp); neither calls
//  anything.
//============================================================================
#include "jassnatives.h"

void SJassRect::SetBounds(SJassBounds bounds)
{
    m_minX = bounds.m_minX;
    m_minY = bounds.m_minY;
    m_maxX = bounds.m_maxX;
    m_maxY = bounds.m_maxY;
}

void SJassRect::Resize(SJassBounds bounds)
{
    m_minX = bounds.m_minX;
    m_minY = bounds.m_minY;
    m_maxX = bounds.m_maxX;
    m_maxY = bounds.m_maxY;
}
