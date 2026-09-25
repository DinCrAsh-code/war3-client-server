//============================================================================
//  0x6F021xxx-0x6F3DAxxx - trivial accessors and type-tag getters.
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
extern void* g_unk6FAAE8D4;   // 0x6FAAE8D4
extern void* g_unk6FA76038;   // 0x6FA76038
extern void* g_unk6FAB558C;   // 0x6FAB558C

//  0x6F0211D0 - returns the constant 0x6162696C, which spells "abil" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F0211D0(void* /*self*/)
{
    return 0x6162696Cu;
}

//  0x6F026790 - returns the constant 0x416E6575, which spells "Aneu" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F026790(void* /*self*/)
{
    return 0x416E6575u;
}

//  0x6F028840 - returns the constant 0x41736569, which spells "Asei" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F028840(void* /*self*/)
{
    return 0x41736569u;
}

//  0x6F0D1AB0 - returns the constant 0x62756666, which spells "buff" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F0D1AB0(void* /*self*/)
{
    return 0x62756666u;
}

//  0x6F2711B0 - returns the constant 0x75697465, which spells "uite" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F2711B0(void* /*self*/)
{
    return 0x75697465u;
}

//  0x6F2AB090 - returns the constant 0x2B773377, which spells "+w3w" read most-significant byte first -
//  CWidget's own agile type id, the free-function twin of the copy CWidget's
//  vtable slot 7 returns (widget_vtable_leaves.cpp), exactly as
//  GetUnitTypeFourCC pairs with CUnit's slot 7 (misc_rawcode_getters.cpp).
//
//  Takes no argument.  It used to be declared __fastcall(void*) on the
//  assumption of a dead `self`; the four typed-slot Assign bodies that call
//  it for real (agenttypedslots.h) set no register at all before the call,
//  which is only consistent with a no-argument function.  Its own two
//  instructions are the same either way, so this costs no score - but a
//  caller compiled against the old declaration would have emitted an extra
//  `mov ecx, ebx` it never had.
unsigned int GetTypeTag_6F2AB090()
{
    return 0x2B773377u;
}

//  0x6F2C73C0 - returns the constant 0x2B773373, which spells "+w3s" read
//  most-significant byte first - CSelectable's own agile type id, the
//  free-function twin of CSelectable's vtable slot 7 (selectable_vtable.cpp).
//  No argument, for the same reason as 0x6F2AB090 just above.
unsigned int GetTypeTag_6F2C73C0()
{
    return 0x2B773373u;
}

//  0x6F301E10 - returns the constant 0x2B63616D, which spells "+cam" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F301E10(void* /*self*/)
{
    return 0x2B63616Du;
}

//  0x6F3DACD0 - returns the constant 0x6763636D, which spells "gccm" read most-significant byte first -
//  a FourCC type tag.
unsigned int __fastcall GetTypeTag_6F3DACD0(void* /*self*/)
{
    return 0x6763636Du;
}

//  0x6F2F5ED0 moved to src/GameUI/gameuigetcamera.cpp as CGameUI::GetCamera
//  once the JASS camera natives established what the field at +0x254 is.

//  0x6F2F73E0 - return the dword at +0x3A8.
int __fastcall GetField0x3A8_6F2F73E0(const void* self)
{
    return *(const int*)((const char*)self + 0x3A8);
}

//  0x6F2F75B0 - return the dword at +0x3B8.
int __fastcall GetField0x3B8_6F2F75B0(const void* self)
{
    return *(const int*)((const char*)self + 0x3B8);
}

//  0x6F021740 - publish the caller's pointer into the g_unk6FAAE8D4 global.
void __fastcall PublishGlobal_6F021740(void* value)
{
    g_unk6FAAE8D4 = value;
}

//  0x6F30A850 - publish the caller's pointer into the g_unk6FA76038 global.
void __fastcall PublishGlobal_6F30A850(void* value)
{
    g_unk6FA76038 = value;
}

//  0x6F30F2F0 - read the g_unk6FAB558C global back.
void* __fastcall GetGlobal_6F30F2F0()
{
    return g_unk6FAB558C;
}
