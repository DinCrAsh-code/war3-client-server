//============================================================================
//  0x6F478C80 - FloatModifier::SetTarget: resolve this modifier's own
//  handle pair and publish the value it is walking towards.
//
//  Eight instructions.  Own translation unit: FloatModifier::Init calls it
//  for real.
//============================================================================
#include "floatpropmodifier.h"

void FloatModifier::SetTarget(const CFloat* value)
{
    ((SFloatSink*)LookupHandle(m_handle, m_typeTag))->SetValue(value);
}
