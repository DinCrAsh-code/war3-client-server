//============================================================================
//  0x6F4F19xx-0x6F4F1Fxx - nine float field setters on one object.
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
//  0x6F4F1920 - store a float into +0x1C8.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x1C8_6F4F1920(void* self, float value)
{
    *(float*)((char*)self + 0x1C8) = value;
}

//  0x6F4F1930 - store a float into +0x1CC.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x1CC_6F4F1930(void* self, float value)
{
    *(float*)((char*)self + 0x1CC) = value;
}

//  0x6F4F1940 - store a float into +0x1D0.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x1D0_6F4F1940(void* self, float value)
{
    *(float*)((char*)self + 0x1D0) = value;
}

//  0x6F4F1950 - store a float into +0x1D4.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x1D4_6F4F1950(void* self, float value)
{
    *(float*)((char*)self + 0x1D4) = value;
}

//  0x6F4F1EE0 - store a float into +0xA8.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0xA8_6F4F1EE0(void* self, float value)
{
    *(float*)((char*)self + 0xA8) = value;
}

//  0x6F4F1EF0 - store a float into +0xAC.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0xAC_6F4F1EF0(void* self, float value)
{
    *(float*)((char*)self + 0xAC) = value;
}

//  0x6F4F1F00 - store a float into +0x15C.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x15C_6F4F1F00(void* self, float value)
{
    *(float*)((char*)self + 0x15C) = value;
}

//  0x6F4F1F10 - store a float into +0x160.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x160_6F4F1F10(void* self, float value)
{
    *(float*)((char*)self + 0x160) = value;
}

//  0x6F4F1F20 - store a float into +0x164.  __fastcall never passes a
//  float in a register, so `value` lands on the stack and the
//  function cleans it - the shipped `retn 4`.
void __fastcall SetFloatField0x164_6F4F1F20(void* self, float value)
{
    *(float*)((char*)self + 0x164) = value;
}
