//============================================================================
//  0x6F7518D0 - write one 8-byte value pair into row `index`'s own +0x20 /
//  +0x24 fields of the 0xA0-byte-per-row table at this+0x2CC, run the
//  table's own per-row prep on it, then set bit 3 of the row's +0x04 flags
//  word and hand back its address.
//
//  Sibling of 0x6F7418E0/0x6F741BE0 - same +0x2CC table, same 0xA0-byte
//  rows (`lea eax,[eax+eax*4]; shl eax,5` = index*5*32), same "OR bit 3
//  into +0x04, return &row+4" tail; 0x6F7418E0 writes +0x18/+0x1C instead
//  of +0x20/+0x24 and 0x6F741BE0 only reads a bit back out of +0x04.
//  Neither sibling is in this batch, so the table's real class and its
//  other fields stay unnamed - offset-addressed rather than guessed at.
//
//  The receiver's real class is not established (no vtable stamp, no
//  allocation tag reachable from this leaf), so it stays generic here.
//
//  Own translation unit: reached from CSelectable's slot 105 with a real
//  call and it makes one of its own.
//============================================================================

struct SAttachmentScaleTable
{
    void* SetSlotPair(int index, const unsigned int* source);

    //  0x6F74A7F0 - zero this row in place (`this` is the row itself, not
    //  the table - called as `((SAttachmentScaleTable*)row)->ResetRow()`
    //  from FindOrReserveSlot below and from an as-yet-unnamed caller,
    //  sub_6F762120).  See attachmentscalerowreset.cpp.
    void ResetRow();

    //  0x6F751780 - the table's own per-row prep, run after the raw value
    //  pair lands and before the flags word is touched.  111 instructions,
    //  no dump body in this batch; naked redirect.  `retn 8` - two stack
    //  arguments (the row's own base address and the same `source` pointer
    //  the caller supplied).
    void PrepRow(void* row, const unsigned int* source);
};

__declspec(naked) void SAttachmentScaleTable::PrepRow(void*,
                                                       const unsigned int*)
{
    __asm
    {
        mov     eax, 06F751780h
        jmp     eax
    }
}

void* SAttachmentScaleTable::SetSlotPair(int index, const unsigned int* source)
{
    unsigned int rowOffset = index * 0xA0;
    char* base = *(char**)((char*)this + 0x2CC);

    *(unsigned int*)(base + rowOffset + 0x20) = source[0];
    *(unsigned int*)(base + rowOffset + 0x24) = source[1];

    //  Reloaded from +0x2CC rather than reusing `base`: the shipped stream
    //  re-reads the field for the call's own row-pointer argument.
    base = *(char**)((char*)this + 0x2CC);
    PrepRow(base + rowOffset, source);

    base = *(char**)((char*)this + 0x2CC);
    *(unsigned int*)(base + rowOffset + 4) |= 8;
    return base + rowOffset + 4;
}
