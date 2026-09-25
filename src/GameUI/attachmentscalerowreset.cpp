//============================================================================
//  0x6F74A7F0 - reached from CItem's own vtable BFS (batch 8 of 8, see
//  docs/targets/CItem-callee-batch8.md) via
//  SAttachmentScaleTable::FindOrReserveSlot (0x6F76F420, out of this
//  batch's scope) and an as-yet-unnamed sibling (sub_6F762120).  A separate
//  translation unit from attachmentscaletable.cpp's own SetSlotPair/PrepRow
//  pair - a different address neighbourhood, so presumably a different
//  original module - but the same class and the same 0xA0-byte row shape
//  (see that file).
//
//  Zeroes the row from +0xC through +0x9C (the row's own end), skipping its
//  header (+0x0-+0xB, set by the caller before/after this call) and a
//  4-byte gap at +0x4C this call tree never touches.  `__thiscall(ecx=row)`
//  - `this` is the row itself, not the table - `retn 0`.
//============================================================================

struct SAttachmentScaleTable
{
    void ResetRow();
};

void SAttachmentScaleTable::ResetRow()
{
    char* base = (char*)this;

    *(float*)(base + 0x0C) = 0.0f;
    *(float*)(base + 0x10) = 0.0f;
    *(float*)(base + 0x14) = 0.0f;
    *(float*)(base + 0x18) = 0.0f;
    *(float*)(base + 0x1C) = 0.0f;
    *(float*)(base + 0x20) = 0.0f;
    *(float*)(base + 0x24) = 0.0f;
    *(int*)(base + 0x28) = 0;
    *(int*)(base + 0x2C) = 0;
    *(float*)(base + 0x30) = 0.0f;
    *(float*)(base + 0x34) = 0.0f;
    *(float*)(base + 0x38) = 0.0f;
    *(float*)(base + 0x3C) = 0.0f;
    *(float*)(base + 0x40) = 0.0f;
    *(float*)(base + 0x44) = 0.0f;
    *(int*)(base + 0x48) = 0;
    *(int*)(base + 0x50) = 0;
    *(int*)(base + 0x54) = 0;
    *(int*)(base + 0x58) = 0;
    *(int*)(base + 0x5C) = 0;
    *(int*)(base + 0x60) = 0;
    *(int*)(base + 0x64) = 0;
    *(int*)(base + 0x68) = 0;
    *(int*)(base + 0x6C) = 0;
    *(int*)(base + 0x70) = 0;
    *(int*)(base + 0x74) = 0;
    *(int*)(base + 0x78) = 0;
    *(int*)(base + 0x7C) = 0;
    *(int*)(base + 0x80) = 0;
    *(int*)(base + 0x84) = 0;
    *(int*)(base + 0x88) = 0;
    *(int*)(base + 0x8C) = 0;
    *(int*)(base + 0x90) = 0;
    *(int*)(base + 0x94) = 0;
    *(int*)(base + 0x98) = 0;
    *(int*)(base + 0x9C) = 0;
}
