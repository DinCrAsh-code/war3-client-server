//============================================================================
//  0x6F4774E0 - SUnitTrackedRef::MakeFloatModifier: make a FloatModifier
//  that walks this tracked value to `value` over `span`, bind it, and hand
//  it back as a counted reference.
//
//  Instruction for instruction the same function as
//  SUnitTrackedRef::MakeBoundaryListener (0x6F477550,
//  floatboundarylistener.cpp) two hundred bytes further on and as
//  CPathRef::MakeMoveModifier (0x6F4741A0, positionmodifiermake.cpp) in the
//  neighbouring module.  See floatboundarylistener.cpp for why the
//  placement new, the unguarded Init on a possibly-null pointer and the
//  hidden-buffer return are each spelled the way they are.
//
//  Its own __FILE__/__LINE__ is ".\\FloatProp.cpp" line 119 - the same file
//  the listener at 105 uses, twelve lines apart in the shipped source.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "floatpropmodifier.h"
#include "storm.h"

static const char kFloatPropCpp[] = ".\\FloatProp.cpp";

//  Slot 5 (+0x14) - the subject, exactly as positionmodifiermake.cpp
//  reaches it.  `retn 0`, nothing pushed.
typedef SModifierSubject* (__thiscall *GetModifierSubjectFn)(void*);

SFloatModifierRef SUnitTrackedRef::MakeFloatModifier(const CFloat* value,
                                                      const CFloat* span)
{
    SModifierSubject* subject =
        ((GetModifierSubjectFn)(*(void***)this)[0x14 / 4])(this);

    FloatModifier* modifier =
        new (SMemAlloc(sizeof(FloatModifier), kFloatPropCpp, 119, 0))
            FloatModifier();

    modifier->Init(subject, this, value, span);

    SFloatModifierRef ref;
    ref.m_ptr = modifier;
    if (modifier)
        modifier->m_refcount++;

    return ref;
}
