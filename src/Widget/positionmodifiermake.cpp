//============================================================================
//  0x6F4741A0 - CPathRef::MakeMoveModifier: make a MovementModifier that
//  walks this position to (x, y) at `rate`, bind it, and hand it back as a
//  counted reference.
//
//  Instruction for instruction the same function as
//  SUnitTrackedRef::MakeBoundaryListener (0x6F477550,
//  floatboundarylistener.cpp) and as SUnitTrackedRef::MakeFloatModifier
//  (0x6F4774E0, floatpropmodifiermake.cpp): slot 5 for the subject, a
//  sixteen-byte Storm block with this module's own __FILE__/__LINE__
//  (".\\Position.cpp", 199), placement new, an *unguarded* Init on the
//  possibly-null result, and the counted reference returned through MSVC's
//  hidden buffer with the conditional addref.  See that file for why each
//  of those is spelled the way it is; none of it is repeated here.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "positionmodifier.h"
#include "storm.h"

static const char kPositionCpp[] = ".\\Position.cpp";

//----------------------------------------------------------------------------
//  Slot 5 (+0x14) on the position's own vtable: the subject the new
//  modifier is bound against.  A hand-written slot dispatch because this
//  repo has no committed vtable for the class - `retn 0`, nothing pushed,
//  which is what the call site itself shows (`mov eax,[edi]` /
//  `mov edx,[eax+14h]` / `call edx` with ecx untouched).
//----------------------------------------------------------------------------
typedef SModifierSubject* (__thiscall *GetModifierSubjectFn)(void*);

SMovementModifierRef CPathRef::MakeMoveModifier(const CFloat* x,
                                                const CFloat* y,
                                                const CFloat* rate)
{
    SModifierSubject* subject =
        ((GetModifierSubjectFn)(*(void***)this)[0x14 / 4])(this);

    MovementModifier* modifier =
        new (SMemAlloc(sizeof(MovementModifier), kPositionCpp, 199, 0))
            MovementModifier();

    //  Not guarded: the shipped code runs Init with `this` null on the
    //  failed-allocation path too (`mov ecx,esi` with esi zeroed).
    modifier->Init(subject, this, x, y, rate);

    SMovementModifierRef ref;
    ref.m_ptr = modifier;
    if (modifier)
        modifier->m_refcount++;

    return ref;
}
