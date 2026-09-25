//============================================================================
//  0x6F40Axxx-0x6F4DExxx - trivial accessors and type-tag getters.
//
//  Trivial accessors: each one is two or three instructions that read or
//  write one field of the object in ecx, or return one constant.  None of
//  them calls anything, so grouping several per translation unit cannot
//  cost an out-of-line call the way it would for the rest of this codebase
//  (CLAUDE.md's one-TU-per-module rule exists to stop cross-inlining, and
//  there is nothing here to inline into anything).
//
//  Declared as free __fastcall functions taking the receiver explicitly
//  rather than as members of a reconstructed class: every one of them puts
//  `this` in ecx and takes no stack argument (or, for the float setters,
//  one float, which MSVC's __fastcall never puts in a register), so a free
//  __fastcall reproduces the shipped signature exactly, and the classes
//  themselves are not established - this session had no live IDA (the
//  ida_query transport is not authorised for this container) and nothing
//  in the call trees names them.  Only the offsets each function actually
//  touches are asserted; no surrounding layout is invented.
//============================================================================
extern void* g_unk6FAB7E98;   // 0x6FAB7E98

//  0x6F40AFA0 - returns the constant 0x2B706C79, which spells "+ply" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F40AFA0(void* /*self*/)
{
    return 0x2B706C79u;
}

//  0x6F40CB90 - returns the constant 0x70697465, which spells "pite" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F40CB90(void* /*self*/)
{
    return 0x70697465u;
}

//  0x6F40B1C0 - return the dword at +0x2A8.
int __fastcall GetField0x2A8_6F40B1C0(const void* self)
{
    return *(const int*)((const char*)self + 0x2A8);
}

//  0x6F41FA00 - return the dword at +0x1E0.
int __fastcall GetField0x1E0_6F41FA00(const void* self)
{
    return *(const int*)((const char*)self + 0x1E0);
}

//  0x6F4A7840 - return the dword at +0x40.
int __fastcall GetField0x40_6F4A7840(const void* self)
{
    return *(const int*)((const char*)self + 0x40);
}

//  0x6F4A95A0 - return the dword at +0x40.
int __fastcall GetField0x40_6F4A95A0(const void* self)
{
    return *(const int*)((const char*)self + 0x40);
}

//  0x6F4CC420 - return the dword at +0x20.
int __fastcall GetField0x20_6F4CC420(const void* self)
{
    return *(const int*)((const char*)self + 0x20);
}

//  0x6F4DE1E0 - return the dword at +0xC.
int __fastcall GetField0xC_6F4DE1E0(const void* self)
{
    return *(const int*)((const char*)self + 0xC);
}

//  0x6F493E80 - set bit mask 0x10000 in the dword at +0xD8.
void __fastcall SetFlag0x10000_6F493E80(void* self)
{
    *(unsigned int*)((char*)self + 0xD8) |= 0x10000u;
}

//  0x6F493E90 - clear bit mask 0x10000 in the dword at +0xD8.
void __fastcall ClearFlag0x10000_6F493E90(void* self)
{
    *(unsigned int*)((char*)self + 0xD8) &= 0xFFFEFFFFu;
}

//  0x6F4BC660 - decrement the dword at +0x34.
void __fastcall DecField0x34_6F4BC660(void* self)
{
    *(unsigned int*)((char*)self + 0x34) -= 1;
}

//  0x6F41F5E0 - empty body (IDA calls it nullsub_16).
void __fastcall NoOp_6F41F5E0()
{
}

//  0x6F4D3330 - publish the caller's pointer into the g_unk6FAB7E98 global.
void __fastcall PublishGlobal_6F4D3330(void* value)
{
    g_unk6FAB7E98 = value;
}
