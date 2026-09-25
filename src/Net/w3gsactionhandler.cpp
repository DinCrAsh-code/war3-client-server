//============================================================================
//  Probably_W3GS_ActionHandler - 0x6F672340 (root of this dump).
//
//  `arg0` is a pointer to a pointer: `self = *arg0` is read once, and if it
//  is null, or the dword at `self+0x54` is zero, the function returns 0
//  immediately without dispatching anything - neither field is otherwise
//  touched here, so nothing in this call tree says what object `self`
//  really is or what its +0x54 field means beyond "must be non-zero to
//  dispatch".
//
//  Otherwise it switches on a single opcode byte (the dump's own `arg_4`,
//  values 0x1D-0x4C / 29-76 - "switch 48 cases" in IDA's own comment) into
//  one of 34 handler functions, forwarding `self` and its own two trailing
//  stack arguments through unchanged. IDA names those two trailing
//  arguments `fdwReason`/`lpvReserved` inside the case bodies - that is type
//  propagation from the handlers' own DllMain-shaped signature (3 args,
//  `retn 0xC`), not evidence this function is itself a DLL entry point; its
//  own signature is 5 stack dwords (`retn 0x14`), the last of which nothing
//  in the body ever reads.
//
//  None of the 34 handlers are reconstructed this session (see
//  w3gsaction_stubs.cpp for the seven trivial ones whose entire body is a
//  single constant return) - only this dispatcher's own shape is
//  established. Named `W3GSAction_<address>` rather than after any inferred
//  opcode meaning: the "2 handlers"/opcode-table ground truth this dump
//  ships with (see its `## Ground truth` section) confirms this function's
//  existence and default-case address (0x6F67264F) but explicitly does not
//  confirm what any individual opcode does.
//============================================================================

int __stdcall _DllMain_at_6F6644E0(void*, unsigned int, unsigned int);
int __stdcall _DllMain_at_6F6644F0(void*, unsigned int, unsigned int);
int __stdcall _DllMain_at_6F664510(void*, unsigned int, unsigned int);
int __stdcall _DllMain_at_6F664520(void*, unsigned int, unsigned int);
int __stdcall _DllMain_at_6F664530(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F664500(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F664550(void*, unsigned int, unsigned int);

//  Not reconstructed this session - only the calling shape (this pointer in
//  the first stack slot, two forwarded dwords, `retn 0xC`) is established,
//  the same convention w3gs_typedispatch.cpp's own not-reconstructed
//  handlers use.
int __stdcall W3GSAction_6F665580(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F667C10(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F667F30(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F667FC0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F667FF0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F668010(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F668030(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66C4B0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66C500(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66DFE0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E0E0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E240(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E300(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E430(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E4F0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E7A0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66E890(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66EAF0(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66ED20(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66EF60(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66F050(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F66FBE0(void*, unsigned int, unsigned int);
//  0x6F66FCA0 - reconstructed as ServerTock_0x27_PacketHandler
//  (servertock_handler.cpp); its second parameter is the packet source,
//  which this dispatcher forwards as the raw dword it receives.
int __stdcall ServerTock_0x27_PacketHandler(void*, const void*, unsigned int);
int __stdcall W3GSAction_6F66FF90(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F670170(void*, unsigned int, unsigned int);
int __stdcall GameAction_0x26_PacketHandler(void*, unsigned int, unsigned int);   // sub_6F671000
int __stdcall W3GSAction_6F671960(void*, unsigned int, unsigned int);
int __stdcall W3GSAction_6F671D20(void*, unsigned int, unsigned int);

int __stdcall Probably_W3GS_ActionHandler(void** arg0, unsigned char opcode,
                                           unsigned int fdwReason, unsigned int lpvReserved,
                                           unsigned int /*unused*/)
{
    void* self = *arg0;
    if (!self || *(int*)((char*)self + 0x54) == 0)
        return 0;

    switch (opcode)
    {
    case 29: return W3GSAction_6F671960(self, lpvReserved, fdwReason);
    case 30: return W3GSAction_6F671D20(self, lpvReserved, fdwReason);
    case 31: return W3GSAction_6F66E240(self, lpvReserved, fdwReason);
    case 32: return W3GSAction_6F66FBE0(self, lpvReserved, fdwReason);
    case 33: return W3GSAction_6F66C4B0(self, lpvReserved, fdwReason);
    case 34: return W3GSAction_6F66E300(self, lpvReserved, fdwReason);
    case 35: return W3GSAction_6F66E430(self, lpvReserved, fdwReason);
    case 36: return W3GSAction_6F66DFE0(self, lpvReserved, fdwReason);
    case 37: return W3GSAction_6F66E0E0(self, lpvReserved, fdwReason);
    case 38: return GameAction_0x26_PacketHandler(self, lpvReserved, fdwReason);
    case 39: return ServerTock_0x27_PacketHandler(self, (const void*)lpvReserved,
                                                  fdwReason);
    case 40: return W3GSAction_6F66E4F0(self, lpvReserved, fdwReason);
    case 41: return W3GSAction_6F66E7A0(self, lpvReserved, fdwReason);
    case 42: return W3GSAction_6F664500(self, lpvReserved, fdwReason);
    case 43: return W3GSAction_6F66E890(self, lpvReserved, fdwReason);
    case 44: return W3GSAction_6F66C500(self, lpvReserved, fdwReason);
    case 45: return W3GSAction_6F66EAF0(self, lpvReserved, fdwReason);
    case 46: return _DllMain_at_6F664520(self, lpvReserved, fdwReason);
    case 52: return W3GSAction_6F665580(self, lpvReserved, fdwReason);
    case 53: return W3GSAction_6F667FC0(self, lpvReserved, fdwReason);
    case 54: return W3GSAction_6F667FF0(self, lpvReserved, fdwReason);
    case 55: return W3GSAction_6F664550(self, lpvReserved, fdwReason);
    case 56: return W3GSAction_6F668010(self, lpvReserved, fdwReason);
    case 57: return W3GSAction_6F668030(self, lpvReserved, fdwReason);
    case 58: return _DllMain_at_6F664510(self, lpvReserved, fdwReason);
    case 59: return W3GSAction_6F667F30(self, lpvReserved, fdwReason);
    case 60: return _DllMain_at_6F664530(self, lpvReserved, fdwReason);
    case 65: return W3GSAction_6F66ED20(self, lpvReserved, fdwReason);
    case 66: return W3GSAction_6F66FF90(self, lpvReserved, fdwReason);
    case 67: return W3GSAction_6F670170(self, lpvReserved, fdwReason);
    case 68: return W3GSAction_6F66EF60(self, lpvReserved, fdwReason);
    case 69: return W3GSAction_6F66F050(self, lpvReserved, fdwReason);
    case 70: return W3GSAction_6F667C10(self, lpvReserved, fdwReason);
    case 75: return _DllMain_at_6F6644E0(self, lpvReserved, fdwReason);
    case 76: return _DllMain_at_6F6644F0(self, lpvReserved, fdwReason);
    default: return 1;
    }
}
