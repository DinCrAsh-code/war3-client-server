//============================================================================
//  0x6F2AE8D0 - CWidget vtable[0x120] (index 72).
//
//  Hand the second argument to a helper, then forward the *negated* float at
//  its own +0x10 to vtable[0x128] together with the first argument, and
//  return the first argument unchanged.
//
//  The negation is an integer `xor` of the sign bit, not an `fchs`: the
//  value is read as a dword, flipped against 0x80000000 and written back
//  into the caller's own second argument slot, which is then passed by
//  address.  That is the same "a float negated without touching the FPU"
//  idiom CFloat relies on everywhere, applied here to an ordinary float.
//
//  Reusing the incoming argument slot as the scratch for the negated value
//  is the shipped code's own choice, and it is why the second parameter is
//  taken by pointer and the negated copy is a separate local here - writing
//  through the parameter itself would change what the caller sees.
//============================================================================
#include "widget.h"

//  0x6F2ADA80 - below this dump's depth cut; thunked.  A thiscall member:
//  `this` in ecx and ONE stack argument, which it cleans itself (`retn 4`).
//  It must NOT be spelled `__fastcall(self, what)`: that parks `what` in edx
//  and pushes nothing, so the real callee reads its `arg_0` off a stack slot
//  this build never wrote - and it does not merely ignore it, it tests it
//  (`mov ebx, [esp+34h+arg_0]` / `test ebx, ebx`) and on a non-zero value
//  dereferences it (`mov ecx, [ebx+4]`) before calling through it.  On top
//  of that the `retn 4` eats four bytes of this function's own frame, so the
//  closing `retn 10h` here returns to whatever that shifted the saved return
//  address to.  Live, that took the hero's attack-animation setup (this is
//  CWidget's slot 0x120) into a DEP violation executing a stack address.
//  Declared through a struct rather than as a free function precisely so the
//  argument lands on the stack and the `ret` counts agree - the same idiom
//  SPlayerTableSubmit::SubmitMasked (widgetregistersubmit.cpp) uses.
struct SWidgetOffsetPrep
{
    void Prepare(void* what);
};

__declspec(naked) void SWidgetOffsetPrep::Prepare(void*)
{
    __asm
    {
        mov     eax, 06F2ADA80h
        jmp     eax
    }
}

typedef void (__thiscall *ApplyOffsetFn)(void*, void*, float*);

void* CWidget::NegateAndForwardOffset(void* target, SWidgetOffsetSource* src,
                                      int, int)
{
    void* vself = this;

    ((SWidgetOffsetPrep*)this)->Prepare(src);

    //  Read as an integer, sign bit flipped, written to a local - never
    //  through `src`, which the caller still owns.
    int negated = *(const int*)&src->m_value ^ (int)0x80000000;

    ((ApplyOffsetFn)(*(void***)vself)[0x128 / 4])(vself, target,
                                                  (float*)&negated);
    return target;
}
