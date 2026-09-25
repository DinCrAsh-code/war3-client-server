//============================================================================
//  Five CDataMgr property-registration wrappers, all reached from the
//  CDataMgr constructor (sub_6F4D85D0, out of this batch's scope - it also
//  stamps CSpriteUber_'s own vtable at the same offset, so it is a much
//  larger body than any of the leaves this dump actually reached).
//
//  Every one of the five is byte-identical in shape: forward its own three
//  stack arguments (entry, slot, flags) straight through to
//  CDataMgr::RegisterManagedProperty (0x6F4E8740, below) with one extra
//  baked constant appended - confirmed against the constructor's own nine
//  call sites, which read as `RegisterX(entry, slot, flags)` with `slot`
//  the incrementing 0..8 array index and `flags` the (usually zero) byte
//  0x6F4E8740 stores at entry->+0xD.
//
//  0x6F4E8740 itself stores the entry into `this->table[slot]` (the table
//  pointer at +0x10 CDataMgr's own TSFixedArray<CBaseManaged*> sizes to
//  nine slots, cdatamgrpropertyarray.cpp), stamps entry->+0xC with the
//  baked kind constant and entry->+0xD with the caller's flags byte, and -
//  only when flags is odd - inserts the entry into a second, time-sorted
//  structure through 0x6F4E84B0.  That insert is a 92-instruction walk over
//  a linked list keyed on a float at +0x18 and is out of this batch's
//  scope; declared and left a thunk, `retn` unconfirmed by dump but not
//  needed here since nothing in this batch calls it directly.
//============================================================================
struct CDataMgr
{
    //  ecx = this, four stack dwords (entry, slot, flags, kind), retn 0x10.
    //  The last two are genuinely single bytes in the shipped listing
    //  (`entry->+0xC`/`+0xD` each take one byte of them) but are declared
    //  `unsigned int` rather than `unsigned char`: MSVC still reserves a
    //  full 4-byte-aligned stack slot per argument either way (confirmed
    //  by this function's own listing, whose four `_name$` offsets are
    //  8/12/16/20 - each exactly 4 apart, not 4/4/1/1), so nothing about
    //  the call sites changes; declaring the true 1-byte width here would
    //  only make thunk_abi_audit.py's own byte count read the last
    //  parameter's declared *type* size instead of its actual slot width
    //  and mis-total the frame as 13 bytes instead of 16.
    void __thiscall RegisterManagedProperty(void* entry, int slot,
                                            unsigned int flags,
                                            unsigned int kind);
};

//  0x6F4E8740 - the shared body all five wrappers below call into.
__declspec(naked) void __thiscall CDataMgr::RegisterManagedProperty(void*, int,
                                                                    unsigned int,
                                                                    unsigned int)
{
    __asm
    {
        mov     eax, 06F4E8740h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  The five wrappers.  `retn 0Ch` each - three stack dwords, thiscall.
//----------------------------------------------------------------------------
struct SManagedPropertyRegistrar1 { void __thiscall Register(void*, int, unsigned int); };
struct SManagedPropertyRegistrar3 { void __thiscall Register(void*, int, unsigned int); };
struct SManagedPropertyRegistrar4 { void __thiscall Register(void*, int, unsigned int); };
struct SManagedPropertyRegistrar5 { void __thiscall Register(void*, int, unsigned int); };
struct SManagedPropertyRegistrar6 { void __thiscall Register(void*, int, unsigned int); };

//  0x6F4E8770 - kind 1.
void __thiscall SManagedPropertyRegistrar1::Register(void* entry, int slot,
                                                      unsigned int flags)
{
    ((CDataMgr*)this)->RegisterManagedProperty(entry, slot, flags, 1);
}

//  0x6F4E87B0 - kind 3.
void __thiscall SManagedPropertyRegistrar3::Register(void* entry, int slot,
                                                      unsigned int flags)
{
    ((CDataMgr*)this)->RegisterManagedProperty(entry, slot, flags, 3);
}

//  0x6F4E87D0 - kind 4.
void __thiscall SManagedPropertyRegistrar4::Register(void* entry, int slot,
                                                      unsigned int flags)
{
    ((CDataMgr*)this)->RegisterManagedProperty(entry, slot, flags, 4);
}

//  0x6F4E87F0 - kind 5.
void __thiscall SManagedPropertyRegistrar5::Register(void* entry, int slot,
                                                      unsigned int flags)
{
    ((CDataMgr*)this)->RegisterManagedProperty(entry, slot, flags, 5);
}

//  0x6F4E8810 - kind 6.
void __thiscall SManagedPropertyRegistrar6::Register(void* entry, int slot,
                                                      unsigned int flags)
{
    ((CDataMgr*)this)->RegisterManagedProperty(entry, slot, flags, 6);
}
