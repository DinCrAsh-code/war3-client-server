//============================================================================
//  0x6F606xxx-0x6F6F9xxx - trivial accessors.
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
extern void* g_unk6FAD1420;   // 0x6FAD1420

//  0x6F606890 - return the dword at +0x140.
int __fastcall GetField0x140_6F606890(const void* self)
{
    return *(const int*)((const char*)self + 0x140);
}

//  0x6F614650 - return the dword at +0x254.
int __fastcall GetField0x254_6F614650(const void* self)
{
    return *(const int*)((const char*)self + 0x254);
}

//  0x6F61E820 - return the dword at +0x1E4.
int __fastcall GetField0x1E4_6F61E820(const void* self)
{
    return *(const int*)((const char*)self + 0x1E4);
}

//  0x6F656FD0 - return the dword at +0x14.
int __fastcall GetField0x14_6F656FD0(const void* self)
{
    return *(const int*)((const char*)self + 0x14);
}

//  0x6F6F8700 - return the dword at +0x1F4.
int __fastcall GetField0x1F4_6F6F8700(const void* self)
{
    return *(const int*)((const char*)self + 0x1F4);
}

//  0x6F6F8A20 - return the dword at +0x204.
int __fastcall GetField0x204_6F6F8A20(const void* self)
{
    return *(const int*)((const char*)self + 0x204);
}

//  0x6F6F8FF0 - return the dword at +0x4.
int __fastcall GetField0x4_6F6F8FF0(const void* self)
{
    return *(const int*)((const char*)self + 0x4);
}

//  0x6F6F92E0 - return the dword at +0x3C.
int __fastcall GetField0x3C_6F6F92E0(const void* self)
{
    return *(const int*)((const char*)self + 0x3C);
}

//  0x6F629750 - decrement the word at +0x2.
void __fastcall DecWord0x2_6F629750(void* self)
{
    *(unsigned short*)((char*)self + 0x2) -= 1;
}

//  0x6F673710 - store 0 into the dword at +0x16C.
void __fastcall ClearField0x16C_6F673710(void* self)
{
    *(unsigned int*)((char*)self + 0x16C) = 0u;
}

//  0x6F673750 - store 0 into the dword at +0x170.
void __fastcall ClearField0x170_6F673750(void* self)
{
    *(unsigned int*)((char*)self + 0x170) = 0u;
}

//  0x6F673790 - store 0 into the dword at +0x174.
void __fastcall ClearField0x174_6F673790(void* self)
{
    *(unsigned int*)((char*)self + 0x174) = 0u;
}

//  0x6F663C70 - empty body (IDA calls it nullsub_1627).  Kept __cdecl and
//  kept the name netprovider_method0x90.cpp already declares it by: that
//  call site is the only caller this repo has, and MSVC mangles the calling
//  convention into the symbol, so a __fastcall respelling here would be a
//  second symbol the linker never resolves.  This replaces the naked
//  pass-through thunk w3gsaction_thunks.cpp used to carry for it.
void __cdecl NullHook_6F663C70()
{
}

//  0x6F6C5B10 - empty body (IDA calls it nullsub_1651).
void __fastcall NoOp_6F6C5B10()
{
}

//  0x6F6F2B60 - returns the constant 0x00000002.
unsigned int __fastcall GetConstant2_6F6F2B60(void* /*self*/)
{
    return 0x00000002u;
}

//  0x6F6C4CB0 - read the g_unk6FAD1420 global back.
void* __fastcall GetGlobal_6F6C4CB0()
{
    return g_unk6FAD1420;
}
