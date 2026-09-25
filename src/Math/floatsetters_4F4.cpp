//============================================================================
//  0x6F4F4Axx - six float field setters on one object.
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
//  0x6F4F4A90 - store a float into +0x18.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x18_6F4F4A90(void* self, float value)
{
    *(float*)((char*)self + 0x18) = value;
}

//  0x6F4F4AA0 - store a float into +0x1C.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x1C_6F4F4AA0(void* self, float value)
{
    *(float*)((char*)self + 0x1C) = value;
}

//  0x6F4F4AC0 - store a float into +0x24.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x24_6F4F4AC0(void* self, float value)
{
    *(float*)((char*)self + 0x24) = value;
}

//  0x6F4F4AD0 - store a float into +0x28.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x28_6F4F4AD0(void* self, float value)
{
    *(float*)((char*)self + 0x28) = value;
}

//  0x6F4F4AE0 - store a float into +0x10.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x10_6F4F4AE0(void* self, float value)
{
    *(float*)((char*)self + 0x10) = value;
}

//  0x6F4F4AF0 - store a float into +0x14.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x14_6F4F4AF0(void* self, float value)
{
    *(float*)((char*)self + 0x14) = value;
}
