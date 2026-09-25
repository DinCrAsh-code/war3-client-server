//============================================================================
//  0x6F7E54xx - three float field setters on one object.  The
//  0x15C/0x160 pair is byte for byte what 0x6F4F1F00/0x6F4F1F10 already
//  are at their own addresses: this image was linked without /OPT:ICF
//  (agentdefaults.cpp), so identical bodies stay separate functions.
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
//  0x6F7E5400 - store a float into +0x158.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x158_6F7E5400(void* self, float value)
{
    *(float*)((char*)self + 0x158) = value;
}

//  0x6F7E5410 - store a float into +0x15C.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x15C_6F7E5410(void* self, float value)
{
    *(float*)((char*)self + 0x15C) = value;
}

//  0x6F7E5420 - store a float into +0x160.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x160_6F7E5420(void* self, float value)
{
    *(float*)((char*)self + 0x160) = value;
}
