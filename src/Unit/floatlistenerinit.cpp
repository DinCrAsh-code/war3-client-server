//============================================================================
//  0x6F480FB0 - FloatListener::Init: everything a freshly allocated
//  boundary listener needs, in shipped order.  The listener copy of
//  FloatModifier::Init (0x6F478CA0, floatpropmodifierinit.cpp) and
//  MovementModifier::Init (0x6F47C7F0).
//
//  Three differences from those two, all real:
//
//   * the mode handed to Bind is not derived from the resolved object
//     alone.  A subject on the synchronised channel (bit 31 of its own
//     flags word) forces mode 2; anything else takes the caller's own
//     `mode` unchanged, which is the `mov eax,2` / `jnz` / `mov eax,[mode]`
//     join and not FloatModifier::Init's unconditional `(flags >> 31) * 2`;
//   * the boundary is published *after* the slot-6 attach rather than
//     before it, and as one call carrying both halves (SetBoundary);
//   * a fourth step the modifiers have no counterpart for: the subject is
//     told, through the +0x14 sub-object's own slot 2
//     (SModifierSubject::AddValueListener, src/Misc/misc_handle_lookups.cpp),
//     which made object now listens, under which message id, on whose
//     behalf.  That is the edge that makes the value reach the caller
//     again when it crosses.
//
//  `resolved` is spilled into this function's own `source` parameter home
//  and read back after Bind returns - MSVC reuses a dead incoming
//  parameter slot as a local here, which is also where the sum in
//  SUnitQueryCircleCentre::EnumerateUnits lives.  Nothing source-visible.
//
//  Own translation unit: three real calls out of it, two into this module.
//============================================================================
#include "floatlistener.h"

void FloatListener::Init(SModifierSubject* subject, SUnitTrackedRef* source,
                         int above, const CFloat* boundary,
                         unsigned int msgId, void* target, int mode)
{
    CHandleObject* resolved = LookupHandle(source->m_handle,
                                           source->m_typeTag);

    Bind(subject, msgId,
         target, (resolved->m_flags & 0x80000000) != 0 ? 2 : mode);

    //  Slot 6 (+0x18): the resolved source object, pushed, `this` in ecx.
    typedef void (__thiscall *AttachSubjectFn)(void*, void*);
    ((AttachSubjectFn)(*(void***)this)[0x18 / 4])(this, resolved);

    SetBoundary(above, boundary);

    subject->AddValueListener(m_handle, msgId, target);
}
