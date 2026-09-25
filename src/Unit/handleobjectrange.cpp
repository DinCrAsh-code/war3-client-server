//============================================================================
//  0x6F4A9390 - SHandleObjectRange::SetRange: publish a new {low, high}
//  window onto a resolved handle object, clamp the current value into it,
//  and notify.
//
//  This is the function every one of SUnitTrackedRef's setters ends in
//  (unittrackedref.cpp): SetLow, SetHigh and SetRange all resolve their
//  handle and hand the pair here.  Was a naked `jmp` thunk in
//  unreconstructed_thunks.cpp.
//
//  Own translation unit: it calls two functions in its own 0x6F4Axxxx
//  module for real, and is itself called from the 0x6F477xxx one.
//
//  The clamp is the *pointer select* idiom CPathTrace::RampValueAt
//  (pathtraceramp.cpp) already records, down to the `add ecx,4` that steps
//  the candidate from the low bound to the high one and the single
//  dereference at the join.  Its comparison senses are the same asymmetric
//  pair too - the low bound wins on strictly greater, the high bound on
//  strictly less - so a value sitting exactly on a bound is left alone.
//  Writing it as two `if`s over the *value* instead makes MSVC duplicate
//  the load and the store into three exits.
//
//  The old value is read and saved *before* the two new bounds are stored,
//  because the notification carries it: `mov eax,[esi+78h]` is the second
//  instruction of the body and its store into the frame happens before the
//  first bound is written.
//============================================================================
#include "handleobjectrange.h"

//  The declaration unittrackedref.cpp calls through is on its own minimal
//  `struct SHandleObjectRange`, and MSVC puts the class name in the
//  mangled name - so the definition has to be on *that* type, not on the
//  fuller one above.  The two are the same object; this one just names the
//  fields.
//  Written naked.  The C++ it replaces is
//
//      SHandleObjectRangeImpl* self = (SHandleObjectRangeImpl*)this;
//      self->PrepareRangeUpdate();
//      CFloat previous = self->m_value;
//      self->m_low  = pair[0];
//      self->m_high = pair[1];
//      const CFloat* pick = &self->m_low;      // the pointer-select clamp,
//      if (*(const float*)&self->m_value.m_bits                 // exactly as
//              < *(const float*)&pick->m_bits) goto done;       // RampValueAt
//      ++pick;
//      if (*(const float*)&self->m_value.m_bits
//              > *(const float*)&pick->m_bits) goto done;
//      pick = &self->m_value;
//  done:
//      self->m_value = *pick;
//      self->NotifyRangeChanged(&self->m_value, &previous, 0);
//
//  - same order, same asymmetric comparison senses, same single
//  dereference at the join - and the eleven it could not reach are all
//  register roles and one hoisted load: the shipped code keeps the value's
//  address in edx and the candidate in ecx (this build has them the other
//  way round, so the `add` and both `fld`s name the other register), and it
//  reads m_value into the frame with a plain `mov` where VC8 hoists an
//  `fld` of it above the two bound stores.  Same two loads, same stores,
//  same branch senses; none of it is source-addressable, so the forty
//  instructions are transcribed.
__declspec(naked) void SHandleObjectRange::SetRange(const CFloat* /*pair*/)
{
    __asm
    {
        push    ecx                         // `previous`
        push    esi
        push    edi
        mov     esi, ecx
        call    SHandleObjectRangeImpl::PrepareRangeUpdate
        mov     eax, [esi+78h]              // the old value, read before
        lea     edx, [esi+78h]              //   either bound is written
        mov     [esp+8], eax
        mov     eax, [esp+10h]              // pair
        mov     edi, [eax]
        lea     ecx, [esi+80h]              // m_low
        mov     [ecx], edi
        mov     eax, [eax+4]
        mov     [ecx+4], eax                // m_high
        fld     dword ptr [edx]
        fld     dword ptr [ecx]
        fcompp
        fnstsw  ax
        test    ah, 41h
        jz      short join
        fld     dword ptr [edx]
        add     ecx, 4                      // step to the high bound
        fld     dword ptr [ecx]
        fcompp
        fnstsw  ax
        test    ah, 5
        jnp     short join
        mov     ecx, edx                    // neither bound: the value
    join:
        mov     ecx, [ecx]
        push    0
        lea     eax, [esp+0Ch]              // &previous
        push    eax
        mov     [edx], ecx                  // m_value = *pick
        push    edx
        mov     ecx, esi
        call    SHandleObjectRangeImpl::NotifyRangeChanged
        pop     edi
        pop     esi
        pop     ecx
        retn    4
    }
}
