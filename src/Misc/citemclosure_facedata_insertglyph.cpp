//============================================================================
//  0x6F7CC450 - FACEDATA_InsertGlyphEntry.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+) - the orchestrator this
//  whole batch's smaller helpers exist to serve.
//
//  `ecx`=table, `edx`=source record, one stack argument (`retn 8`) an
//  out-pointer for the resulting error code.  Loads the entry via
//  FACEDATA_LoadFaceEntry; on success, either installs it as an owner
//  hook (FACEDATA_InstallOwnerHookRecord) when the source's own bit 3 is
//  set and it resolves a valid owner descriptor, or - failing that -
//  scans a small array of installed-hook candidates at that owner's own
//  `+0x0C`/count-of-4-each for the first whose own `+0` byte 0 is set,
//  reusing it (again via InstallOwnerHookRecord) or falling back to a
//  fixed slot when the scan runs out.  A failed load or install path
//  releases the entry (FACEDATA_ReleaseFieldViaEdi then
//  FACEDATA_RemoveGlyphEntry) and answers whatever error the failing step
//  produced.  On overall success it allocates and links a 0x0C-byte
//  binding record (FACEDATA_AllocateAndZero, FACEDATA_LinkTail), acquires
//  the owner binding (FACEDATA_AcquireOwnerBinding) and creates the
//  glyph's own entry (FACEDATA_CreateGlyphEntry), stamps a fresh
//  {8/0xC/0x10000-shaped} block at `table+0x80`, and answers 0.
//
//  `__declspec(naked)`: it calls three naked callees whose own real
//  parameters are registers with no exposed calling convention -
//  FACEDATA_LoadFaceEntry (`esi`), FACEDATA_ReleaseFieldViaEdi (`edi`) and
//  FACEDATA_ComputeGlyphMetrics is not reached from here but
//  FACEDATA_AcquireOwnerBinding's own `ecx`/`edx` are ordinary - so the
//  whole body is transcribed literally rather than split, the same reason
//  every other naked function in this closure gives.
//============================================================================
#include "storm.h"

void FACEDATA_LoadFaceEntry();
void __stdcall InsertListEntry(void);
int __fastcall FACEDATA_InstallOwnerHookRecordRaw(void);   // see note below
void __stdcall FACEDATA_ReleaseFieldViaEdi(void*);
int __fastcall FACEDATA_RemoveGlyphEntry(void* self);
int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_LinkTail(void* list, void* node);
void FACEDATA_AcquireOwnerBinding();
int __fastcall FACEDATA_CreateGlyphEntry(void* self, void** outPtr);

//  FACEDATA_InstallOwnerHookRecord is declared `__stdcall` with six named
//  C++ parameters elsewhere (Misc/citemclosure_facedata_installhook.cpp);
//  calling it from naked `__asm` needs only the right decorated symbol,
//  which that declaration already produces, so it is named directly below
//  rather than through the placeholder above (kept only so the `#include`
//  order here does not have to match that file's own struct definitions).
struct SFaceHookOwnerFwd;
int __stdcall FACEDATA_InstallOwnerHookRecord(void* table, void* ctx, int a,
                                               int b, int c, void** outPtr);

__declspec(naked) void FACEDATA_InsertGlyphEntry()
{
    __asm {
        sub     esp, 0x1C
        mov     eax, [esp + 0x1C + 8]
        push    ebx
        xor     ebx, ebx
        cmp     eax, ebx
        push    esi
        push    edi
        mov     esi, edx
        mov     edi, ecx
        mov     [esp + 0x28 - 0x18], ebx
        mov     [esp + 0x28 - 0x0C], ebx
        jz      return_six

        cmp     esi, ebx
        jz      return_six

        mov     [eax], ebx
        test    byte ptr [esi], 2
        jz      flag_default
        cmp     [esi + 0x10], ebx
        mov     byte ptr [esp + 0x28 - 0x19], 1
        jnz     have_flag
flag_default:
        mov     byte ptr [esp + 0x28 - 0x19], bl
have_flag:
        push    ebp
        lea     eax, [esp + 0x2C - 0x14]
        push    eax
        push    edi
        call    FACEDATA_LoadFaceEntry
        mov     ebp, eax
        test    ebp, ebp
        jnz     final_epilogue

        mov     edx, [esi]
        test    dl, 8
        mov     ecx, [edi]
        mov     [esp + 0x2C - 4], ecx
        jz      no_owner_hook

        mov     ebp, [esi + 0x14]
        test    ebp, ebp
        jz      no_owner_hook

        mov     eax, [ebp + 0]
        test    byte ptr [eax], 1
        jz      install_direct

        xor     eax, eax
        xor     ecx, ecx
        test    dl, 0x10
        jz      no_hook_arg
        mov     eax, [esi + 0x18]
        mov     ecx, [esi + 0x1C]
no_hook_arg:
        lea     edx, [esp + 0x2C - 0x18]
        push    edx
        push    ecx
        mov     ecx, [esp + 0x34 - 0x14]
        push    eax
        mov     eax, [esp + 0x38 + 4]
        push    eax
        push    ecx
        push    ebp
        call    FACEDATA_InstallOwnerHookRecord
        mov     ebp, eax
        xor     esi, esi
        cmp     ebp, esi
        jz      success_common

        movzx   edx, byte ptr [esp + 0x2C - 0x19]
        mov     ebx, [esp + 0x2C - 0x18]
        push    edx
        lea     edi, [esp + 0x30 - 0x14]
        call    FACEDATA_ReleaseFieldViaEdi
        mov     ecx, ebx
        call    FACEDATA_RemoveGlyphEntry
        mov     eax, ebp
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        add     esp, 0x1C
        retn    8

install_direct:
        movzx   edx, byte ptr [esp + 0x2C - 0x19]
        push    edx
        lea     edi, [esp + 0x30 - 0x14]
        mov     ebp, 0x20
        call    FACEDATA_ReleaseFieldViaEdi
        mov     ecx, ebx
        call    FACEDATA_RemoveGlyphEntry
        mov     eax, ebp
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        add     esp, 0x1C
        retn    8

no_owner_hook:
        mov     ecx, [edi + 0x0C]
        lea     eax, [edi + 0x10]
        lea     ecx, [eax + ecx * 4]
        cmp     eax, ecx
        mov     [esp + 0x2C - 0x10], eax
        mov     [esp + 0x2C - 8], ecx
        jnb     scan_exhausted

        mov     ebx, [esp + 0x2C + 4]
        mov     edi, [esp + 0x2C - 0x14]
scan_loop:
        mov     edx, [eax]
        mov     ebp, [edx]
        test    byte ptr [ebp + 0], 1
        jz      scan_advance

        xor     eax, eax
        xor     ecx, ecx
        test    byte ptr [esi], 0x10
        jz      no_scan_arg
        mov     eax, [esi + 0x18]
        mov     ecx, [esi + 0x1C]
no_scan_arg:
        lea     ebp, [esp + 0x2C - 0x18]
        push    ebp
        push    ecx
        push    eax
        push    ebx
        push    edi
        push    edx
        call    FACEDATA_InstallOwnerHookRecord
        mov     ebp, eax
        test    ebp, ebp
        jz      scan_found_none
        cmp     ebp, 2
        jnz     failure_common

        mov     eax, [esp + 0x2C - 0x10]
        mov     ecx, [esp + 0x2C - 8]
scan_advance:
        add     eax, 4
        cmp     eax, ecx
        mov     [esp + 0x2C - 0x10], eax
        jb      scan_loop
        mov     ebx, [esp + 0x2C - 0x18]
scan_exhausted:
        movzx   eax, byte ptr [esp + 0x2C - 0x19]
        push    eax
        lea     edi, [esp + 0x30 - 0x14]
        call    FACEDATA_ReleaseFieldViaEdi
        mov     ecx, ebx
        mov     ebp, 2
        call    FACEDATA_RemoveGlyphEntry
        mov     eax, ebp
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        add     esp, 0x1C
        retn    8

scan_found_none:
        xor     esi, esi
success_common:
        cmp     byte ptr [esp + 0x2C - 0x19], 0
        mov     ebx, [esp + 0x2C - 0x18]
        jz      no_slow_flag
        or      dword ptr [ebx + 8], 0x4000
no_slow_flag:
        mov     ecx, [esp + 0x2C - 4]
        lea     edx, [esp + 0x2C - 0x0C]
        push    edx
        mov     edx, 0x0C
        call    FACEDATA_AllocateAndZero
        mov     ebp, eax
        cmp     ebp, esi
        jnz     failure_common

        mov     edx, [esp + 0x2C - 0x0C]
        mov     [edx + 8], ebx
        mov     ecx, [ebx + 0x60]
        add     ecx, 0x18
        call    FACEDATA_LinkTail
        lea     edx, [esp + 0x2C + 4]
        mov     ecx, ebx
        call    FACEDATA_AcquireOwnerBinding
        mov     ebp, eax
        cmp     ebp, esi
        jnz     failure_common

        mov     eax, [esp + 0x2C + 4]
        lea     edx, [esp + 0x2C + 4]
        mov     ecx, ebx
        mov     [ebx + 0x54], eax
        call    FACEDATA_CreateGlyphEntry
        mov     ebp, eax
        cmp     ebp, esi
        jnz     failure_common

        mov     ecx, [esp + 0x2C + 4]
        mov     edx, [esp + 0x2C + 8]
        mov     [ebx + 0x58], ecx
        mov     eax, [ebx + 0x80]
        mov     ecx, 0x10000
        mov     [eax + 8], esi
        mov     [eax + 0x0C], esi
        mov     [eax + 0x14], esi
        mov     [eax + 0x18], esi
        mov     [eax + 4], ecx
        mov     [eax + 0x10], ecx
        mov     eax, ebp
        pop     ebp
        pop     edi
        pop     esi
        mov     [edx], ebx
        pop     ebx
        add     esp, 0x1C
        retn    8

failure_common:
        mov     ebx, [esp + 0x2C - 0x18]
        mov     ecx, ebx
        call    FACEDATA_RemoveGlyphEntry
final_epilogue:
        mov     eax, ebp
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        add     esp, 0x1C
        retn    8

return_six:
        pop     edi
        pop     esi
        mov     eax, 6
        pop     ebx
        add     esp, 0x1C
        retn    8
    }
}
