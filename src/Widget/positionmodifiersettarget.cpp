//============================================================================
//  0x6F47C790 - MovementModifier::SetTarget: publish the destination point.
//
//  Both axes go through the module's own "flush to zero under a threshold"
//  bit trick before they are stored, the same one
//  SHandleWithType::FlushedOriginDelta (timesyncbounds.cpp) applies to its
//  own pair and which that file deliberately leaves as a literal
//  transliteration rather than a named helper - nothing in either call tree
//  pins down what it is testing well enough to name it, and it does not
//  reduce to any already-named CFloat idiom.
//
//  The pair is built as one two-word local whose address is handed on, not
//  as two pushes: the shipped code carves eight bytes, stores into both and
//  pushes one `lea`.
//
//  Own translation unit: MovementModifier::Init calls it for real.
//============================================================================
#include "positionmodifier.h"

void MovementModifier::SetTarget(const CFloat* x, const CFloat* y)
{
    int bx = x->m_bits;
    int by = y->m_bits;

    CGridVec2 point(kCFloatNoInit);
    point.m_x = CFloat(~(((bx - 0x3000000) ^ bx) >> 31) & (bx + 0xFD800000));
    point.m_y = CFloat(~(((by - 0x3000000) ^ by) >> 31) & (by + 0xFD800000));

    ((SPositionSink*)LookupHandle(m_handle, m_typeTag))->SetTarget(&point);
}
