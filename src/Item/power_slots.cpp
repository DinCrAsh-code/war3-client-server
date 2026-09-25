//============================================================================
//  CPower's four dispatcher overrides (slots 148/151/152/153) and the four
//  brand-new leaf slots they call through (190-193).
//
//  All four dispatchers share one shape, confirmed by reading all four
//  bodies (0x6F0294C0/0x6F029470/0x6F029430/0x6F0293F0) side by side: call
//  this instance's own new leaf slot with a subset of the incoming stack
//  args, and if it answers nonzero, return that answer verbatim; otherwise
//  fall back to the inherited CAbility::EvaluateDepCheckAlias(0) (slot 154,
//  ability_depcheckalias.cpp - reached here through [this-vtable+0x268],
//  unchanged from CAbility/CAbilityInterfaced) and translate its 0/nonzero
//  result into a return code via the compiler's own "neg eax / sbb eax,eax
//  / and eax,0xFFFFFF48 / add eax,0xB8" lowering of `cond ? 0 : 0xB8` (0
//  when the fallback answered yes, 0xB8 when it answered no - 0xFFFFFF48
//  is exactly -0xB8 mod 2^32, the standard branchless-ternary spelling for
//  a two-constant result already documented in docs/msvc-vc8-idioms.md).
//
//  Which leaf each dispatcher calls, and with which of its own arguments,
//  read directly off each body's own indirect-call site:
//    slot 148 (Method_6F052850, 4 args) -> vtable+0x2F8 (slot 190, 2 args):
//      Method_0x2F8(a0, a2)
//    slot 151 (Method_6F052820, 5 args) -> vtable+0x300 (slot 192, 3 args):
//      Method_0x300(a0, a2, a3)
//    slot 152 (Method_6F052810, 4 args) -> vtable+0x2FC (slot 191, 2 args):
//      Method_0x2FC(a0, a2)
//    slot 153 (Method_6F052800, 3 args) -> vtable+0x304 (slot 193, 1 arg):
//      Method_0x304(a0)
//  Every dispatcher ignores its own second stack argument (a1) entirely -
//  never read anywhere in any of the four bodies.
//
//  CPower's own four leaf slots (190-193) are plain "not yet overridden"
//  defaults - each ignores every argument and `this`, and always answers
//  the fixed sentinel 0xDD (221) - exactly the same "mov eax, 0xDD; retn n"
//  shape CAbility's own default-answer accessor family (ability.h's
//  DefaultIntFieldQueryA..AS) already uses for the identical purpose one
//  level down.  A derived class (CAbilityButton is known to override at
//  least one - see CAbilityButton's own reconstruction) is expected to
//  replace one or more of the four with a real per-instance answer.
//============================================================================
#include "power.h"

int CPower::Method_0x2F8(int, int)
{
    return 0xDD;
}

int CPower::Method_0x2FC(int, int)
{
    return 0xDD;
}

int CPower::Method_0x300(int, int, int)
{
    return 0xDD;
}

int CPower::Method_0x304(int)
{
    return 0xDD;
}

//----------------------------------------------------------------------------
//  0x6F0294C0 - slot 148, overriding CAbility::Method_6F052850.
//----------------------------------------------------------------------------
int CPower::Method_6F052850(int a0, int /*a1*/, int a2, int /*a3*/)
{
    int answer = Method_0x2F8(a0, a2);
    if (answer != 0)
        return answer;
    return EvaluateDepCheckAlias(0) ? 0 : 0xB8;
}

//----------------------------------------------------------------------------
//  0x6F029470 - slot 151, overriding CAbility::Method_6F052820.
//----------------------------------------------------------------------------
int CPower::Method_6F052820(int a0, int /*a1*/, int a2, int a3, int /*a4*/)
{
    int answer = Method_0x300(a0, a2, a3);
    if (answer != 0)
        return answer;
    return EvaluateDepCheckAlias(0) ? 0 : 0xB8;
}

//----------------------------------------------------------------------------
//  0x6F029430 - slot 152, overriding CAbility::Method_6F052810.
//----------------------------------------------------------------------------
int CPower::Method_6F052810(int a0, int /*a1*/, int a2, int /*a3*/)
{
    int answer = Method_0x2FC(a0, a2);
    if (answer != 0)
        return answer;
    return EvaluateDepCheckAlias(0) ? 0 : 0xB8;
}

//----------------------------------------------------------------------------
//  0x6F0293F0 - slot 153, overriding CAbility::Method_6F052800.
//----------------------------------------------------------------------------
int CPower::Method_6F052800(int a0, int /*a1*/, int /*a2*/)
{
    int answer = Method_0x304(a0);
    if (answer != 0)
        return answer;
    return EvaluateDepCheckAlias(0) ? 0 : 0xB8;
}
