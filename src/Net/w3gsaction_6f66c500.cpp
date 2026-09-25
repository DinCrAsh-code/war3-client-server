//============================================================================
//  0x6F66C500 - Probably_W3GS_ActionHandler's case 44. Same undeclared
//  extra-register-argument hazard w3gsaction_6f66c4b0.cpp already documents
//  for its own sibling (case 33, the address right before this one): the
//  shipped body's first instruction after the prologue pushes is
//  `mov edi, ecx`, and `edi` is later used as the `this` pointer for a call
//  into `sub_6F66A460`. Same DIFFERS reasoning; not to be hooked without
//  confirming what a live caller leaves in ecx here.
//
//  The field parse's own "out" pointer targets the no-fields action
//  (ParseNoFieldsAction_6F666820, which never writes through it), so a
//  throwaway local stands in for the shipped body's own reused `self` stack
//  slot.
//
//  `CheckAndAdvanceInterval`'s own `outLongSilence` out-pointer targets the
//  same stack slot this function's own status local (`status`) uses, and
//  `allowEscalate` is 0 for this call, so - exactly as in
//  w3gsaction_6f667f30.cpp - that pointer is never actually written and
//  `status` is always 2 by the time the final `cmp ..., 2` gate is reached
//  (the gate always takes its "call through" arm, matching the shipped
//  body's own always-taken branch here).
//============================================================================

int __fastcall ParseNoFieldsAction_6F666820(void* src, unsigned int declaredLength, void* out);
int __fastcall CheckAndAdvanceInterval(int* state, unsigned int intervalMs, int allowEscalate, int* outLongSilence);

//  0x6F66A460 - reconstructed for real (batch-G-netevent-and-tail) in its
//  own TU, w3gsaction_6f66a460.cpp - see that file's own header.  This
//  caller discards the real function's own `unsigned int` return value,
//  matching the shipped call site (no `test eax,eax` or store follows it).
extern unsigned int __fastcall Method_6F66A460_Real(void* self, void* payload);

int __stdcall W3GSAction_6F66C500(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenEcx;
    __asm { mov hiddenEcx, ecx }

    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    void* unusedOut;
    if (!ParseNoFieldsAction_6F666820((void*)arg4, arg8, &unusedOut))
        return 1;

    int status = 2;
    int due = CheckAndAdvanceInterval((int*)((char*)self + 0x50), 0x1F4, 0, &status);
    if (due || status == 2)
    {
        Method_6F66A460_Real(hiddenEcx, &arg8);
        status = 2;
    }

    return status;
}
