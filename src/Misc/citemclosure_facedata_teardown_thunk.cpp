//============================================================================
//  0x6F7CB760 - FACEDATA record teardown.  CItem vtable slots 32/104
//  closure, batch K continuation.  Not itself part of this session's
//  worklist and left a redirect: its own callees (0x6F7C9AC0, a release
//  loop over `esi+0x54`; 0x6F7CA980, taking `sub_6F7C9BA0` as a raw
//  callback pointer) are both below this session's depth cut, and a real
//  reconstruction here would only compile against thunked callees of its
//  own - the same "reconstruct the leaf first" rule this whole closure
//  otherwise follows.  `__fastcall(ecx=owner, edx=record)`, one stack
//  argument (`retn 4`), read straight off the dump's own trailing `retn`.
//============================================================================
__declspec(naked) void __fastcall TeardownFaceRecord(void* owner, void* record,
                                                       void* recordAlias)
{
    __asm {
        mov eax, 06F7CB760h
        jmp eax
    }
}
