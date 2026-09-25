//============================================================================
//  0x6F494460 / 0x6F494480 / 0x6F4937E0 - three tiny setters on a packed
//  dword bitfield at `this+0xD8` (the first two) and a plain dword copy at
//  `this+0xBC` (the third), reached only through the 0x6F28B0xx thiscall
//  adjustor thunks (misc_adjuster_thunks.cpp's own pattern - `this` is
//  already biased by 0x164 there, and those four thunks themselves are not
//  yet reconstructed: an adjustor thunk followed inline by real logic under
//  one IDA heading needs raw asm this session didn't have time to get
//  right; deferred) - the owning class is not otherwise identified in this
//  closure, so these are modelled as bare-offset operations on an opaque
//  `this` rather than named struct members.
//============================================================================

struct SPackedFieldD8Owner
{
    //  0x6F494460 - field D8's byte 1 (bits 8-15).
    void __thiscall SetPackedFieldD8Byte1(unsigned char value);

    //  0x6F494480 - field D8's nibble 3 (bits 12-15).
    void __thiscall SetPackedFieldD8Nibble3(unsigned char value);

    //  0x6F4937E0 - copy *src into this+0xBC.
    void __thiscall SetFieldBCFromPointer(const int* src);
};

void __thiscall SPackedFieldD8Owner::SetPackedFieldD8Byte1(unsigned char value)
{
    unsigned int* field = (unsigned int*)((char*)this + 0xD8);
    *field = (*field & 0xFFFFF0FFu) | ((unsigned int)value << 8);
}

void __thiscall SPackedFieldD8Owner::SetPackedFieldD8Nibble3(unsigned char value)
{
    unsigned int* field = (unsigned int*)((char*)this + 0xD8);
    *field = (*field & 0xFFFF0FFFu) | ((unsigned int)value << 12);
}

void __thiscall SPackedFieldD8Owner::SetFieldBCFromPointer(const int* src)
{
    *(int*)((char*)this + 0xBC) = *src;
}
