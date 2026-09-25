//============================================================================
//  Three more members of the same +0x2C8/+0x2CC attachment-scale table
//  SAttachmentScaleTable::SetSlotPair operates on (0x6F7518D0,
//  attachmentscaletable.cpp), all reached through vtable slot dispatch from
//  CSelectable's slots 101/105.  Each carries either a full
//  `__except_handler4`-shaped SEH frame or a `/GS`-style stack cookie this
//  toolchain cannot reproduce under the fixed `/GS- /EHs-c-` build flags
//  (docs/msvc-vc8-idioms.md), or - for the grow path - a chunk-rounding
//  division and a shift-copy this batch's confidence bar does not cover;
//  all three stay naked redirects.
//============================================================================

struct SAttachmentScaleTable
{
    //  0x6F71D1F0 - build "SelectionCircleTexture%s%02d"-shaped config-file
    //  lookups over the table and read back a size/texture selection.  A
    //  `/GS`-style stack cookie (`xor eax,esp` against dword_6FAAE140,
    //  checked through sub_6F7E1059 on the way out) - unreproducible.
    //  `retn 8` - two stack arguments.
    void QueryTextureSelection(int, int);

    //  0x6F76F420 - find or reserve a slot in the table (a linear scan for
    //  a matching or first-empty row, then - if none - a chunk-rounded grow
    //  through a hardware `div`, the same shape TSGrowableArray::SetAlloc's
    //  own chunk math has). `retn 14h` - five stack arguments.
    void FindOrReserveSlot(int, int, int, int, int);

    //  0x6F76D870 - format a shadow-texture path for a slot and load it,
    //  behind the same unreproducible stack-cookie frame QueryTextureSelection
    //  carries. `retn 8` - two stack arguments.
    void LoadShadowTexture(int, int);
};

__declspec(naked) void SAttachmentScaleTable::QueryTextureSelection(int, int)
{
    __asm
    {
        mov     eax, 06F71D1F0h
        jmp     eax
    }
}

__declspec(naked) void SAttachmentScaleTable::FindOrReserveSlot(int, int, int,
                                                                 int, int)
{
    __asm
    {
        mov     eax, 06F76F420h
        jmp     eax
    }
}

__declspec(naked) void SAttachmentScaleTable::LoadShadowTexture(int, int)
{
    __asm
    {
        mov     eax, 06F76D870h
        jmp     eax
    }
}
