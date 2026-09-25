//============================================================================
//  0x6F537xxx-0x6F5CBxxx - two trivial slots.
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
//  0x6F537B10 - store 1 into the dword at +0xAD0.
void __fastcall SetField0xAD0_6F537B10(void* self)
{
    *(unsigned int*)((char*)self + 0xAD0) = 1u;
}

//  0x6F5CB840 - empty body, three stack arguments it never reads
//  (the shipped `retn 0Ch` pops them).
void __stdcall NoOp3Args_6F5CB840(int, int, int)
{
}
