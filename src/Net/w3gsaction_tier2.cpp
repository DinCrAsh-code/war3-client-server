//============================================================================
//  batch-B-w3gsaction_thunks - five more of Probably_W3GS_ActionHandler's own
//  opcode cases, all the same shape: forward (self, arg4, arg8) straight
//  into a shared per-record-shape validator, appending one more
//  caller-supplied constant (the declared record size the validator checks
//  the caller's own length field against) as that validator's own trailing
//  argument. `retn 0Ch` here (three stack arguments in, matching every
//  other handler in this dispatcher family) - the extra constant is pushed
//  fresh for the call, not part of this function's own signature.
//
//  The validators themselves (sub_6F67B430/sub_6F67BA70/sub_6F680560/
//  sub_6F67E3D0) are not addresses this batch's own worklist entry names,
//  and each is substantial (61-341 instructions) - out of this session's
//  scope, same "whole-function naked redirect to the real, unhooked body"
//  reasoning w3gsaction_6f66c4b0.cpp's own Method_6F66A140 already
//  establishes for a callee in the same position. Confirmed `retn 10h`/
//  `retn 14h` (matching the 4/5 arguments each pushes) directly off each
//  one's own dump tail.
//============================================================================

//  0x6F67B430 - retn 10h (4 args: self, arg4, arg8, recordSize).
extern "C" __declspec(naked) int __stdcall Validator_6F67B430(void*, unsigned int, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F67B430 }
    __asm { jmp eax }
}

//  0x6F67BA70 - retn 10h, same 4-argument shape.
extern "C" __declspec(naked) int __stdcall Validator_6F67BA70(void*, unsigned int, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F67BA70 }
    __asm { jmp eax }
}

//  0x6F680560 - retn 10h, same 4-argument shape.
extern "C" __declspec(naked) int __stdcall Validator_6F680560(void*, unsigned int, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F680560 }
    __asm { jmp eax }
}

//  0x6F67E3D0 - retn 14h (5 args: self, arg4, arg8, 0, recordSize).
extern "C" __declspec(naked) int __stdcall Validator_6F67E3D0(void*, unsigned int, unsigned int, unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F67E3D0 }
    __asm { jmp eax }
}

//  0x6F67B640 - case (see w3gsactionhandler2.cpp). Record size 0.
int __stdcall W3GSAction2_6F67B640(void* self, unsigned int arg4, unsigned int arg8)
{
    return Validator_6F67B430(self, arg4, arg8, 0);
}

//  0x6F67BB60 - record size 0.
int __stdcall W3GSAction2_6F67BB60(void* self, unsigned int arg4, unsigned int arg8)
{
    return Validator_6F67BA70(self, arg4, arg8, 0);
}

//  0x6F680600 - record size 0.
int __stdcall W3GSAction2_6F680600(void* self, unsigned int arg4, unsigned int arg8)
{
    return Validator_6F680560(self, arg4, arg8, 0);
}

//  0x6F67E8B0 - record size 0x48.
int __stdcall W3GSAction2_6F67E8B0(void* self, unsigned int arg4, unsigned int arg8)
{
    return Validator_6F67E3D0(self, arg4, arg8, 0, 0x48);
}

//  0x6F67E8D0 - record size 0x0C.
int __stdcall W3GSAction2_6F67E8D0(void* self, unsigned int arg4, unsigned int arg8)
{
    return Validator_6F67E3D0(self, arg4, arg8, 0, 0x0C);
}
