//============================================================================
//  0x6F47C7F0 - MovementModifier::Init: everything a freshly allocated
//  modifier needs, in shipped order.
//
//  Resolve the *source* position's own handle pair (+0x08/+0x0C, the pair
//  CPathRef carries), turn its sync-channel bit into the 0-or-2 `mode`
//  Bind takes (`flags >> 31` doubled, which is the shipped `shr`/`add`
//  and not a `? 2 : 0`), bind, tell slot 6 about the resolved object, then
//  publish the rate and the destination.
//
//  The rate substitution is a hardware-float equality against g_CFloatZero,
//  not one of CFloat's own comparison members: the shipped code does
//  `fld [ecx]` / `fld g_CFloatZero` / `fucompp`, which the FPU can do
//  directly on these bit patterns, and the branch that follows *keeps* the
//  caller's pointer when the two differ.  A zero rate means "use the
//  module's own default ramp" instead.
//
//  Own translation unit: four real calls out of it, three of them into
//  this same module.
//============================================================================
#include "positionmodifier.h"

void MovementModifier::Init(SModifierSubject* subject, CPathRef* source,
                            const CFloat* x, const CFloat* y,
                            const CFloat* rate)
{
    CHandleObject* resolved = LookupHandle(source->m_handle,
                                           source->m_typeTag);

    Bind(subject, (int)((resolved->m_flags >> 31) + (resolved->m_flags >> 31)));

    //  Slot 6 (+0x18): the resolved source object, pushed, `this` in ecx.
    typedef void (__thiscall *AttachSubjectFn)(void*, void*);
    ((AttachSubjectFn)(*(void***)this)[0x18 / 4])(this, resolved);

    const CFloat* effective = rate;
    if (*(const float*)&rate->m_bits == *(const float*)&g_CFloatZero.m_bits)
        effective = g_pDefaultModifierRamp;
    SetRate(effective);

    SetTarget(x, y);
}
