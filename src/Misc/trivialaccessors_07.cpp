//============================================================================
//  0x6F71Axxx-0x6F7B8xxx - trivial accessors.
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
//  dword_6FA4F6E8 is g_rowTableCurrentRow (rowtable.cpp), already an int
//  there - the same spelling and the same type, or the linker sees two
//  different symbols.
extern int g_rowTableCurrentRow;   // 0x6FA4F6E8
extern void* g_unk6FADF154;   // 0x6FADF154
extern void* g_unk6FADF158;   // 0x6FADF158

//  0x6F7B43F0 - decrement the dword at +0x24.
void __fastcall DecField0x24_6F7B43F0(void* self)
{
    *(unsigned int*)((char*)self + 0x24) -= 1;
}

//  0x6F71A6D0 - read the g_rowTableCurrentRow global back.
int __fastcall GetGlobal_6F71A6D0()
{
    return g_rowTableCurrentRow;
}

//  0x6F7B8B50 - read the g_unk6FADF154 global back.
void* __fastcall GetGlobal_6F7B8B50()
{
    return g_unk6FADF154;
}

//  0x6F7B8B60 - read the g_unk6FADF158 global back.
void* __fastcall GetGlobal_6F7B8B60()
{
    return g_unk6FADF158;
}
