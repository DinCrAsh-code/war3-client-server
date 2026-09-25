//============================================================================
//  0x6F700FC0 - look up (or create) a field-lookup record off `this+0x10C`
//  (sub_6F6FF010, out of this session's own closure), format the caller's
//  own path string into it (Storm_501), load a World Editor Database sheet
//  from that path (LoadWorldEditorDatabase, Misc/citemclosure_wedb_depth7.cpp
//  - a naked `__cdecl()` redirect with no declared parameters, since the
//  real body reads `ecx`/`edx` directly rather than through any spelling
//  this compiler's calling-convention keywords reach), and mark `this`
//  loaded (`+0x198 = 1`).  Written naked rather than as ordinary C++
//  specifically so `ecx`/`edx` can be staged immediately ahead of that
//  redirect exactly the way the shipped code does - an ordinary call
//  expression has no way to pass arguments through a zero-parameter
//  `__cdecl` declaration.
//============================================================================
//  0x6F6FF010 - out of this session's own closure; declared exactly as
//  this file's own inline-asm call site pushes it (`lea ecx,[esi+10Ch]` /
//  `call sub_6F6FF010`, so ecx = the field-lookup slot) and given a real
//  naked-redirect body here so the .mix actually links - not a
//  reconstruction, just the address this file's own call already needs.
__declspec(naked) void* __fastcall sub_6F6FF010(void* self)
{
    __asm { mov eax, 06F6FF010h }
    __asm { jmp eax }
}
void  __stdcall  Storm_501(char* dest, const char* src, unsigned int size);
void  __cdecl    LoadWorldEditorDatabase();

struct SSheetFieldTable;

__declspec(naked) void __fastcall LoadSheetFieldAndArm(SSheetFieldTable* /*ecx*/,
                                                        void* /*edx, unused*/,
                                                        const char* /*path*/)
{
    __asm {
        push ebx
        push esi
        mov esi, ecx
        push edi
        lea ecx, [esi+10Ch]
        call sub_6F6FF010
        mov ebx, [esp+10h]
        push 7FFFFFFFh
        mov edi, eax
        push ebx
        lea eax, [edi+4]
        push eax
        call Storm_501
        xor edx, edx
        mov ecx, ebx
        call LoadWorldEditorDatabase
        mov [edi], eax
        pop edi
        mov dword ptr [esi+198h], 1
        pop esi
        pop ebx
        retn 4
    }
}
