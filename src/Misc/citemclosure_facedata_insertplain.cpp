//============================================================================
//  0x6F7CC8C0 - FACEDATA_InsertPlainGlyphEntry.  CItem vtable slots
//  32/104 closure, batch K continuation (depth 12-25+) - the last of this
//  batch's 34 addresses.
//
//  A thin wrapper over FACEDATA_InsertGlyphEntry: builds a temporary
//  12-byte "plain record" source (flags=1, i.e. bit 0 set - the
//  FACEDATA_LoadFaceEntry field-initialised-record case - followed by the
//  two init values `edx` and this function's own first stack argument)
//  and forwards it, `table` (`ecx`, untouched the whole way through) and
//  its own remaining two stack arguments (context, outPtr) straight into
//  it.  `edx+6` for a null `edx` - the same `lea`-not-`mov` idiom
//  FACEDATA_ScaleGlyphSize's own null guards use - without touching
//  `table` or building anything.
//
//  `__declspec(naked)`: FACEDATA_InsertGlyphEntry is itself naked with the
//  same `ecx`/`edx`/stack shape (see that file's own header), so calling
//  it correctly needs the identical register discipline this function's
//  own shipped body already has.
//============================================================================
void FACEDATA_InsertGlyphEntry();

__declspec(naked) void FACEDATA_InsertPlainGlyphEntry()
{
    __asm {
        sub     esp, 0x20
        test    edx, edx
        jnz     have_edx
        lea     eax, [edx + 6]
        add     esp, 0x20
        retn    0x0C

have_edx:
        mov     eax, [esp + 0x20 + 4]
        mov     [esp + 0x20 - 0x1C], edx
        mov     edx, [esp + 0x20 + 0x0C]
        mov     [esp + 0x20 - 0x18], eax
        mov     eax, [esp + 0x20 + 8]
        push    edx
        push    eax
        lea     edx, [esp + 0x28 - 0x20]
        mov     dword ptr [esp + 0x28 - 0x20], 1
        call    FACEDATA_InsertGlyphEntry
        add     esp, 0x20
        retn    0x0C
    }
}
