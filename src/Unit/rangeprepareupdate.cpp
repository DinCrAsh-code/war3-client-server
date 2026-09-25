//============================================================================
//  0x6F4A90B0 - SHandleObjectRangeImpl::PrepareRangeUpdate, the first thing
//  SHandleObjectRange::SetRange (handleobjectrange.cpp) does.  A naked
//  redirect there until now.
//
//  **The object is a CPathTrace.**  handleobjectrange.cpp had already
//  pinned its +0x78/+0x80/+0x84 as {value, low, high}; game.h names the
//  same three words RampBase()/RampMin()/RampMax() and records the
//  evidence (0x6F4A8FD0 calls GetTravelDistance and RampValueAt on one
//  `this`).  This body settles it: it calls both of those and
//  RefreshSyncSample, all three CPathTrace's own.
//
//  So what it "prepares" is the ramp's baseline - re-evaluate the ramp at
//  however far the move has got by now, make that the new base, and put
//  the sync sample back in step.  Only then does SetRange overwrite the
//  bounds, which is why this has to run first: past the store the old
//  ramp is gone.
//
//  Own translation unit.  SetRange calls it for real, so it must not be
//  inlined into it - which is what kept it a naked body in that file until
//  there was a real one to move out.
//
//  The tail is a tail call: `mov ecx, esi` / `add esp, 8` / `jmp
//  0x6F497440`, which is what a `void` call in return position compiles
//  to and is why RefreshSyncSample is last here.
//============================================================================
#include "handleobjectrange.h"
#include "game.h"

//  Written naked.  The C++ it replaces is
//
//      CPathTrace* trace = (CPathTrace*)this;
//      CFloat rebased(kCFloatNoInit);       // RampValueAt fills it first
//      const CFloat& travelled = trace->GetTravelDistance();
//      trace->RampValueAt(&rebased, &travelled);
//      trace->RampBase() = rebased;
//      trace->RefreshSyncSample();
//
//  and every call, argument, store and branch of it is right; the six
//  instructions it could not reach are the two recorded below, both
//  measured and neither source-addressable.  So the seventeen are
//  transcribed.
__declspec(naked) void SHandleObjectRangeImpl::PrepareRangeUpdate()
{
    __asm
    {
        sub     esp, 8                      // rebased, and the return buffer
        push    esi
        lea     eax, [esp+8]                // GetTravelDistance's buffer
        push    eax
        mov     esi, ecx
        call    CPathTrace::GetTravelDistance
        push    eax                         // the pointer it returned
        lea     ecx, [esp+8]                // &rebased
        push    ecx
        mov     ecx, esi
        call    CPathTrace::RampValueAt
        mov     edx, [esp+4]
        mov     [esi+78h], edx              // RampBase() = rebased
        mov     ecx, esi
        pop     esi
        add     esp, 8
        jmp     CPathTrace::RefreshSyncSample
    }
}

//============================================================================
//  What the compiled C++ could not reach, and what was tried - kept,
//  because it is the evidence for why the body above is naked.
//
//  Eleven of seventeen, and the six misses were two things, both measured:
//
//  * **the two temporaries sit in each other's slots.**  The shipped frame
//    has GetTravelDistance's return buffer at -4 and `rebased` at -8; this
//    build has them the other way round, so every `lea` names the other
//    slot.  Declaring `rebased` ahead of `travelled` was tried and changes
//    nothing - the return buffer is a *compiler* temporary ($S1$) and its
//    slot is not source-addressable, which is the case CLAUDE.md names
//    outright.  With it goes the shipped `push eax`: this build recomputes
//    `&travelled` with a `lea` where the original re-uses the pointer
//    GetTravelDistance left in eax.  CPathTrace::RampFractionAt
//    (pathtraceramp.cpp) has the identical pair of `lea`s off the identical
//    source idiom, which is what says this is the toolchain and not the
//    spelling.
//  * **the last call is not a tail call.**  The shipped body ends
//    `pop esi` / `add esp, 8` / `jmp 0x6F497440`; this one keeps `esi` live
//    to the call and returns after it.  Same call, same receiver, same
//    absence of arguments and of a return value.
//
//  No call, argument, store or branch differs.
//============================================================================
