//============================================================================
//  0x6F478C50 - FloatModifier::SetSpan: resolve this modifier's own handle
//  pair and hand the span, plus two flags, to whatever it names.
//
//  Twelve instructions.  Its one caller only ever passes (span, 1, 0); the
//  other two are still parameters because 0x6F4A9ED0 reads both (`retn 0Ch`
//  with three stack arguments, and its own body branches on each).
//
//  Own translation unit: FloatModifier::Init calls it for real.
//============================================================================
#include "floatpropmodifier.h"

void FloatModifier::SetSpan(const CFloat* span, int a, int b)
{
    ((SFloatSink*)LookupHandle(m_handle, m_typeTag))->SetSpan(span, a, b);
}
