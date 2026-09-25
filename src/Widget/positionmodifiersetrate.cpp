//============================================================================
//  0x6F47C770 - MovementModifier::SetRate: resolve this modifier's own
//  handle pair and publish the travel rate on whatever it names.
//
//  Ten instructions, and the two zeros after the rate are the shipped
//  immediates - 0x6F4A8450 takes three stack arguments (`retn 0Ch`) and
//  this caller only ever varies the first.
//
//  Own translation unit: MovementModifier::Init calls it for real.
//============================================================================
#include "positionmodifier.h"

void MovementModifier::SetRate(const CFloat* rate)
{
    ((SPositionSink*)LookupHandle(m_handle, m_typeTag))->SetRate(rate, 0, 0);
}
