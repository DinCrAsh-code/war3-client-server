//============================================================================
//  0x6F7C98E0/0x6F7C99D0 - FACEDATA_AcquireOwnerHook/FACEDATA_ReleaseOwnerHook.
//  CItem vtable slots 32/104 closure, batch K continuation (depth 12-25+).
//
//  Both `__declspec(naked)`: `edi`/`esi` (`this`) arrive already set, with
//  no `mov edi/esi, ecx` anywhere in the shipped body - another pair of
//  whole-program-optimised internal helpers whose one caller each in this
//  closure (0x6F7C9A30/FACEDATA_AcquireOwnerBinding, itself below, keeps
//  both live across its own calls).
//
//  AcquireOwnerHook allocates a 0x20-byte slot at `this+0x94`, registers
//  it (InsertListEntry, Misc/citemclosure_batchL_thunks.cpp) unless the
//  owner's own flags already say bit 0x200, and - only once both succeed -
//  tail-jumps into an optional owner-descriptor callback at `+0x40`
//  instead of returning normally, so the caller's own frame is the
//  callback's.  ReleaseOwnerHook is its teardown: an optional pre-release
//  callback at `+0x44`, an optional slot release at `+0x58`, an optional
//  ReleaseListChain (same thunk file) unless that same flag bit is set,
//  and a tail-jump into FACEDATA_ReleaseIfSet on `this+0x94` for its own
//  return value and stack cleanup.
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);
void __stdcall InsertListEntry(void);
void __stdcall ReleaseListChain(void);

__declspec(naked) void FACEDATA_AcquireOwnerHook()
{
    __asm {
        push    ecx
        mov     eax, [edi + 4]
        push    ebx
        push    ebp
        push    esi
        mov     esi, [eax + 0x60]
        mov     ecx, [esi + 4]
        mov     ebx, [esi + 8]
        mov     ebp, [esi + 0x14]
        lea     edx, [esp + 0x10 - 4]
        mov     [edi], ecx
        push    edx
        mov     edx, 0x20
        mov     ecx, ebx
        call    FACEDATA_AllocateAndZero
        test    eax, eax
        jnz     epilogue

        mov     edx, [esp + 0x10 - 4]
        mov     [edi + 0x94], edx
        mov     ecx, [esi]
        test    dword ptr [ecx], 0x200
        jnz     have_flag
        mov     ecx, ebx
        call    InsertListEntry
        test    eax, eax
        jnz     epilogue
have_flag:
        mov     edx, [ebp + 0x40]
        test    edx, edx
        jz      epilogue

        pop     esi
        pop     ebp
        mov     ecx, edi
        pop     ebx
        add     esp, 4
        jmp     edx

epilogue:
        pop     esi
        pop     ebp
        pop     ebx
        pop     ecx
        retn
    }
}

__declspec(naked) void FACEDATA_ReleaseOwnerHook()
{
    __asm {
        mov     eax, [esi + 4]
        push    ebx
        push    edi
        mov     edi, [eax + 0x60]
        mov     eax, [edi + 0x14]
        mov     eax, [eax + 0x44]
        test    eax, eax
        mov     ebx, [edi + 8]
        jz      no_predtor
        mov     ecx, esi
        call    eax
no_predtor:
        test    byte ptr [esi + 0x0C], 1
        jz      no_slot_release
        lea     edx, [esi + 0x58]
        mov     ecx, ebx
        call    FACEDATA_ReleaseIfSet
no_slot_release:
        mov     ecx, [edi]
        test    dword ptr [ecx], 0x200
        jnz     skip_release_chain
        mov     edx, [esi + 0x94]
        mov     ecx, [edx]
        call    ReleaseListChain
        mov     eax, [esi + 0x94]
        mov     dword ptr [eax], 0
skip_release_chain:
        pop     edi
        mov     ecx, ebx
        lea     edx, [esi + 0x94]
        pop     ebx
        jmp     FACEDATA_ReleaseIfSet
    }
}
