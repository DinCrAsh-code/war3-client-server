//============================================================================
//  CUnit batch-14 (slot-3 closure, off the message dispatcher 0x6F2A7E60).
//
//  0x6F00F670 / 0x6F00F770 - two small forwarders into the Storm render-
//  primitive singleton (GetStormSingletonA, 0x6F01F5A0, already
//  reconstructed - Storm/stormsingletona.h) plus one of its own methods.
//  Both callees (0x6F76DE70 / 0x6F74D450) open a real __except_handler4
//  frame at their own address - the same SEH-frame ceiling
//  docs/targets/CUnit__vtable.md already documents sixteen times over -
//  so both stay thunked in cunitclosure_batch14_thunks.cpp, declared as
//  members of a proxy over the singleton so `this` lands in `ecx` and the
//  stack args push in the exact right-to-left order each dump's own
//  `arg_0`/`arg_4`/`arg_8` reads name.  Reached from both
//  RefreshPositionNotifyState's own closure (0x6F283830, already thunked)
//  and 0x6F2ADA52 (ReleaseAndRefreshFootprintOrientation's own tree) -
//  neither caller names what these particular Storm calls publish, so
//  named for the shape (a position-like int triple / a lone float) rather
//  than guessed at further.
//
//  0x6F00F670 - `retn 4`.  `ecx`/`edx` are two live register arguments
//  that survive unread through the whole body (the shipped stream never
//  assumes ecx survives a call - docs/msvc-vc8-idioms.md - so it is saved
//  onto the stack before the intervening GetStormSingletonA() call and
//  restored from there for the real forward); the one stack argument is a
//  third int.  All three, plus the singleton, are handed straight to
//  0x6F76DE70.
//
//  0x6F00F770 - `retn 4`.  Same shape, one live register argument (`ecx`,
//  saved onto the stack the same way while the float value that
//  physically occupies its saved slot gets built there instead) and one
//  stack float argument, both forwarded to 0x6F74D450.
//============================================================================
#include "stormsingletona.h"

//  0x6F01F5A0 - already reconstructed, Storm/lazysingleton.cpp.
SStormSingletonA* __fastcall GetStormSingletonA();

class CStormSingletonAThunkProxy
{
public:
    void Method_6F76DE70(int /*arg0*/, int /*arg4*/, int /*arg8*/);
    void Method_6F74D450(int /*arg0*/, float /*arg4*/);
};

//  `naked` can only be applied to a non-member function definition, so the
//  two redirects are defined out of line - same shape
//  GameUI/attachmenttextureformat.cpp's own SAttachmentScaleTable members
//  use.
__declspec(naked) void CStormSingletonAThunkProxy::Method_6F76DE70(
    int /*arg0*/, int /*arg4*/, int /*arg8*/)
{
    __asm { mov eax, 06F76DE70h }
    __asm { jmp eax }
}

__declspec(naked) void CStormSingletonAThunkProxy::Method_6F74D450(
    int /*arg0*/, float /*arg4*/)
{
    __asm { mov eax, 06F74D450h }
    __asm { jmp eax }
}

void __fastcall ForwardPositionTripleToStormSingleton(int p1, int p2, int p3)
{
    ((CStormSingletonAThunkProxy*)GetStormSingletonA())->Method_6F76DE70(
        p1, p2, p3);
}

void __fastcall ForwardFloatToStormSingleton(int p1, int /*p2, unused*/,
                                             float value)
{
    ((CStormSingletonAThunkProxy*)GetStormSingletonA())->Method_6F74D450(
        p1, value);
}
