//============================================================================
//  0x6F7CB4C0 - FACEDATA_LoadFaceEntry.  CItem vtable slots 32/104 closure,
//  batch K continuation (depth 12-25+).
//
//  `__declspec(naked)`: `esi` (the source record describing where the
//  entry's data comes from - a file, an embedded blob, or a shared
//  reference) arrives already set, with no `mov esi, ecx/edx` anywhere in
//  the shipped body - another whole-program-optimised internal helper
//  whose one caller in this closure (0x6F7CC450, this session's own
//  FACEDATA_InsertGlyphEntry, below its own depth cut) keeps it live
//  across the call.  `table` (the owning allocator/table pointer) and
//  `outPtr` are real stack parameters (`retn 8`).
//
//  Allocates a fresh 0x28-byte record through `table`, stamps its own
//  +0x1C with `table`'s own +0x00 field, then branches on `esi`'s low
//  bits: bit 0 - a plain field-initialised record (FACEDATA_InitRecord);
//  bit 2 - load from a file (OpenFaceFileStream), releasing the fresh
//  allocation on a failed open; bit 1 with a non-null `esi[0x10]` - reuse
//  an already-open handle, releasing the fresh allocation since it is not
//  needed; neither bit set - error 6, funnelled through the *file* path's
//  own release-and-store tail (not the bit-1 path's), which the shipped
//  `jmp` target says directly.  0x21 for a null `table`, unconditionally,
//  and `esi+6` for a null `esi` (a literal `lea`, not a plain `mov eax,6`
//  - preserved because the shipped code spells it that way).
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);
void __fastcall FACEDATA_InitRecord(void* srcVtablish, void* field00Value,
                                     void* field04Value, void* out);
int __fastcall OpenFaceFileStream(const char* fileName, void* out);

__declspec(naked) void FACEDATA_LoadFaceEntry()
{
    __asm {
        push    ecx
        mov     eax, [esp + 8]          ; table (arg_0)
        test    eax, eax
        jnz     have_table
        mov     eax, 0x21
        pop     ecx
        retn    8

have_table:
        test    esi, esi
        jnz     have_source
        lea     eax, [esi + 6]
        pop     ecx
        retn    8

have_source:
        mov     ecx, [esp + 0x0C]       ; outPtr (arg_4)
        push    ebx
        push    ebp
        mov     dword ptr [ecx], 0
        mov     ebp, [eax]
        lea     edx, [esp + 8]          ; &var_4
        push    edx
        mov     edx, 0x28
        mov     ecx, ebp
        call    FACEDATA_AllocateAndZero
        mov     ebx, eax
        test    ebx, ebx
        jnz     final_epilogue
        push    edi
        mov     edi, [esp + 0x0C]       ; var_4: the freshly allocated record
        mov     [edi + 0x1C], ebp
        mov     eax, [esi]
        test    al, 1
        jz      not_plain_record

        mov     eax, [esi + 8]
        mov     edx, [esi + 4]
        mov     ecx, [esp + 0x14]       ; table (arg_0), re-read
        push    edi
        push    eax
        call    FACEDATA_InitRecord
        mov     edx, [esp + 0x18]       ; outPtr (arg_4)
        mov     [edx], edi
        pop     edi
        pop     ebp
        mov     eax, ebx
        pop     ebx
        pop     ecx
        retn    8

not_plain_record:
        test    al, 4
        jz      not_file_record

        mov     ecx, [esi + 0x0C]       ; FileName
        mov     edx, edi
        call    OpenFaceFileStream
        mov     ecx, [esi + 0x0C]
        mov     ebx, eax
        test    ebx, ebx
        mov     [edi + 0x10], ecx
        jz      store_and_return
release_alloc_and_reload_edi:
        lea     edx, [esp + 0x0C]       ; &var_4
        mov     ecx, ebp
        call    FACEDATA_ReleaseIfSet
        mov     edi, [esp + 0x0C]       ; var_4, now released
store_and_return:
        mov     edx, [esp + 0x18]       ; outPtr (arg_4)
        mov     [edx], edi
        pop     edi
final_epilogue:
        pop     ebp
        mov     eax, ebx
        pop     ebx
        pop     ecx
        retn    8

not_file_record:
        test    al, 2
        jz      unknown_source_kind
        cmp     dword ptr [esi + 0x10], 0
        jz      unknown_source_kind

        lea     edx, [esp + 0x0C]       ; &var_4
        mov     ecx, ebp
        call    FACEDATA_ReleaseIfSet
        mov     edi, [esi + 0x10]
        mov     edx, [esp + 0x18]       ; outPtr (arg_4)
        mov     [edx], edi
        pop     edi
        pop     ebp
        mov     eax, ebx
        pop     ebx
        pop     ecx
        retn    8

unknown_source_kind:
        mov     ebx, 6
        jmp     release_alloc_and_reload_edi
    }
}
