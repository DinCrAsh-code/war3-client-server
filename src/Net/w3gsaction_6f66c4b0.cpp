//============================================================================
//  0x6F66C4B0 - Probably_W3GS_ActionHandler's case 33. Same undeclared
//  extra-register-argument hazard w3gsaction_6f667c10.cpp already documents
//  for case 70, this time in `ecx` rather than `edx`: the shipped body's
//  very first instruction after the prologue pushes is `mov esi, ecx`, and
//  `esi` is later used as the `this` pointer for a call into `sub_6F66A140`.
//  Nothing in Probably_W3GS_ActionHandler's own reconstructed body
//  (w3gsactionhandler.cpp) sets ecx before this call - it never touches ecx
//  at all - so whatever reaches this handler is whatever *its own* caller
//  (outside this bounded call tree) left there. Modelled the same way:
//  ecx captured literally via an inline `mov`, DIFFERS rather than
//  IDENTICAL for the same reason. Not to be hooked without confirming what
//  a live caller leaves in ecx here.
//============================================================================

int __fastcall ParseDwordAction_6F6664B0(void* src, unsigned int declaredLength, unsigned int* out);

//  0x6F66A140 - out of this batch's own scope (80 instructions behind its
//  own SEH frame, not one of the 60 addresses this session's worklist
//  names) - only the calling shape is established here: `__fastcall(this,
//  void* edx)`, `retn` with nothing to clean (confirmed from its own dump's
//  tail). Whole-function naked redirect to the real, unhooked body, the
//  same reasoning already-established out-of-scope callees elsewhere in
//  this tree use (see e.g. netdata_loadreplayheader.cpp's own
//  Sub_6F7E2EF0).
extern "C" __declspec(naked) void __fastcall Method_6F66A140(void* self, void* edx)
{
    __asm { mov eax, 0x6F66A140 }
    __asm { jmp eax }
}

int __stdcall W3GSAction_6F66C4B0(void* self, unsigned int arg4, unsigned int arg8)
{
    void* hiddenEcx;
    __asm { mov hiddenEcx, ecx }

    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned int parsed;
    if (!ParseDwordAction_6F6664B0((void*)arg4, arg8, &parsed))
        return 1;

    *(unsigned int*)((char*)self + 0x44) = parsed;
    Method_6F66A140(hiddenEcx, &self);

    return 0;
}
