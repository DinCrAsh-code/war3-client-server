//============================================================================
//  0x6F478CA0 - FloatModifier::Init: everything a freshly allocated float
//  modifier needs, in shipped order.  The FloatProp copy of
//  MovementModifier::Init (0x6F47C7F0, positionmodifierinit.cpp).
//
//  Two differences from that one, both real:
//
//   * the target value is published *before* the span, not after;
//   * the "is the span zero" test is not a bare compare.  The span is first
//     run through `- g_CFloatZero`, which is the software float's own
//     denormal flush, then its sign bit is masked off, and the magnitude is
//     compared against the module's threshold flt_6FAAE614 on the FPU.  So
//     "close enough to zero", not "equal to zero" - and only then is the
//     module's default ramp substituted.
//
//  The magnitude is read back out of the very slot operator- wrote it into
//  (the shipped `mov eax,[eax]` on the returned buffer address), which is
//  what constructing a CFloat straight from the masked bits gives and what
//  a named intermediate does not.
//
//  Own translation unit: four real calls out of it, three into this module.
//============================================================================
#include "floatpropmodifier.h"

void FloatModifier::Init(SModifierSubject* subject, SUnitTrackedRef* source,
                         const CFloat* value, const CFloat* span)
{
    CHandleObject* resolved = LookupHandle(source->m_handle,
                                           source->m_typeTag);

    Bind(subject, (int)((resolved->m_flags >> 31) * 2));

    SetTarget(value);

    CFloat magnitude((*span - g_CFloatZero).m_bits & 0x7FFFFFFF);

    const CFloat* effective = span;
    if (*(const float*)&magnitude.m_bits < g_CFloatEqualityTolerance)
        effective = g_pDefaultModifierRamp;

    SetSpan(effective, 1, 0);

    //  Slot 6 (+0x18): the resolved source object, pushed, `this` in ecx.
    typedef void (__thiscall *AttachSubjectFn)(void*, void*);
    ((AttachSubjectFn)(*(void***)this)[0x18 / 4])(this, resolved);
}
