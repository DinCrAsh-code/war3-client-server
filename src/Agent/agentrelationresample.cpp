//============================================================================
//  0x6F4A9E20 - CDrivenRelation::Resample: ask the trace where it is now
//  and push that value through Update().
//
//  The two calls on the trace are the pair game.h says only a CPathTrace
//  ever answers - GetTravelDistance() and RampValueAt() - and they are
//  spelled the way SUnitTrackedRef::GetValue (unittrackedref.cpp) spells
//  the identical pair: the distance is bound as a reference and the pointer
//  operator returned is pushed straight into RampValueAt's argument list
//  rather than being dereferenced into a second local.
//
//  With no trace, or with one that has set its own retired bit, there is
//  nothing left to sample and the channel registration goes instead - as a
//  real tail jump, which is what the shipped epilogue-then-`jmp` is and
//  what writing the release path last (so it is the out-of-line arm) gives.
//
//  Own translation unit: it calls GetTrace() and Update() for real, and
//  both setters call it for real.
//============================================================================
#include "agentrelation.h"

//----------------------------------------------------------------------------
//  +0x4F bit 0 - the trace's own "stop sampling me" bit, tested as a byte
//  because the shipped code tests it as one.  Reached through a cast rather
//  than by naming a field in CPathTrace's reserved block: nothing else in
//  this repo touches it, and game.h is included nearly everywhere.
//----------------------------------------------------------------------------
//  (the naked body below tests the byte directly; the helper is kept as a
//  comment rather than as a function with no caller)
//
//      static int TraceIsRetired(const CPathTrace* trace)
//      {
//          return ((const unsigned char*)trace)[0x4F] & 1;
//      }

//  Written naked.  The C++ it replaces is
//
//      CPathTrace* trace = GetTrace();
//      if (trace != 0 && !TraceIsRetired(trace))
//      {
//          const CFloat& travelled = trace->GetTravelDistance();
//          CFloat value(kCFloatNoInit);
//          trace->RampValueAt(&value, &travelled);
//          Update(&value);
//          return;
//      }
//      ReleaseRegistration();
//
//  and the two instructions it could not reach are both the same kind of
//  register bookkeeping: the shipped code passes RampValueAt the pointer
//  GetTravelDistance *returned* (`push eax`) rather than re-forming the
//  local's address, and it tail-jumps to ReleaseRegistration with `this`
//  still in ecx instead of reloading it from edi.  The second one is safe
//  as well as faithful here: GetTrace (agentrelationdrivefields.cpp) is the
//  two-instruction `mov eax,[ecx+40h]` / `retn` the shipped function is,
//  byte for byte, and never writes ecx - so ecx really does still hold
//  `this` on both paths inside the .mix.
__declspec(naked) void CDrivenRelation::Resample()
{
    __asm
    {
        sub     esp, 8                      // value, and the return buffer
        push    esi
        push    edi
        mov     edi, ecx
        call    CDrivenRelation::GetTrace   // preserves ecx (see above)
        mov     esi, eax
        test    esi, esi
        jz      short release
        test    byte ptr [esi+4Fh], 1       // the trace's retired bit
        jnz     short release
        lea     eax, [esp+0Ch]
        push    eax
        mov     ecx, esi
        call    CPathTrace::GetTravelDistance
        push    eax                         // the pointer it returned
        lea     ecx, [esp+0Ch]
        push    ecx
        mov     ecx, esi
        call    CPathTrace::RampValueAt
        lea     edx, [esp+8]
        push    edx
        mov     ecx, edi
        call    CDrivenRelation::Update
        pop     edi
        pop     esi
        add     esp, 8
        retn
    release:
        pop     edi
        pop     esi
        add     esp, 8
        jmp     CAgentRelation::ReleaseRegistration
    }
}
