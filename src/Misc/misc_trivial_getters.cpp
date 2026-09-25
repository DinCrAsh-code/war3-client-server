//============================================================================
//  A batch of small, self-contained leaves pulled in as callees of the
//  CMissileThunderBolt impact-handler closures - trivial getters/setters
//  and bare FourCC-constant returns.  None of these has its owning class
//  otherwise identified in this closure, so each is a free function on a
//  raw offset into an opaque `this`.
//============================================================================

//  0x6F156770 - a bare FourCC constant.
#include <math.h>

unsigned int GetFourCC_6F156770()
{
    return 0x41747267;
}

//  0x6F113CC0 - another bare FourCC constant.
unsigned int GetFourCC_6F113CC0()
{
    return 0x42706C79;
}

//  0x6F4F08D0 - plain getter, field +0x1C.
struct SField1COwner { int __thiscall GetField1C(); };
int __thiscall SField1COwner::GetField1C()
{
    return *(int*)((char*)this + 0x1C);
}

//  0x6F481430 - plain getter, field +0x238.
struct SField238Owner { int __thiscall GetField238(); };
int __thiscall SField238Owner::GetField238()
{
    return *(int*)((char*)this + 0x238);
}

//  0x6F481410 - plain decrement, field +0x230 - `CTimeSync`'s own scratch-
//  object borrow cursor (see Agent/poposcl_relatedagentslot21.cpp's own
//  `BorrowScratchRelatedList`, the increment this pairs with). `this` in
//  ecx, no stack arguments, `retn`.
struct SField230Owner { void __thiscall DecrementField230(); };
void __thiscall SField230Owner::DecrementField230()
{
    *(unsigned int*)((char*)this + 0x230) -= 1;
}

//  0x6F2C74A0 - plain setter, field +0x50 cleared to 0.
struct SField50Owner { void __thiscall ClearField50(); };
void __thiscall SField50Owner::ClearField50()
{
    *(int*)((char*)this + 0x50) = 0;
}

//  0x6F629730 / 0x6F629740 - single-byte in-place increment/decrement at
//  `this+0`, no other fields touched.  From the CObserver module
//  (0x6F62Axxx); not otherwise identified beyond the shape.
struct SByteCounterOwner
{
    void __thiscall IncrementByte0();
    void __thiscall DecrementByte0();
};
void __thiscall SByteCounterOwner::IncrementByte0()
{
    *(unsigned char*)this += 1;
}
void __thiscall SByteCounterOwner::DecrementByte0()
{
    *(unsigned char*)this -= 1;
}

//  0x6F2F3220 - OR a stack argument into field +0x284, then unconditionally
//  set field +0x26C to 1.
struct SField284Owner { void __thiscall OrField284SetField26C(unsigned int mask); };
void __thiscall SField284Owner::OrField284SetField26C(unsigned int mask)
{
    *(unsigned int*)((char*)this + 0x284) |= mask;
    *(int*)((char*)this + 0x26C) = 1;
}

//  0x6F501D80 - field +0x98 is a sub-object pointer; if null, answer false,
//  else answer bit 0x10 of the sub-object's own +0x54.
struct SField98Owner { int __thiscall TestSubObjectFlag10(); };
int __thiscall SField98Owner::TestSubObjectFlag10()
{
    char* sub = *(char**)((char*)this + 0x98);
    if (!sub)
        return 0;
    return *(int*)(sub + 0x54) & 0x10;
}

//  0x6F26F9E0 - test one bit (stack argument, 0-31) of the 64 bits spanning
//  fields +0x148/+0x14C.
struct SBitfield148Owner { int __thiscall TestBit(int bitIndex); };
int __thiscall SBitfield148Owner::TestBit(int bitIndex)
{
    int combined = *(int*)((char*)this + 0x14C) | *(int*)((char*)this + 0x148);
    return (combined & (1 << bitIndex)) > 0;
}

//  0x6F252980 - a 6-column row/col table lookup into a shared data array.
//
//  Deliberately NOT `extern "C"`.  A C-linkage extern makes MASM emit
//  `_dword_6FAB3C08` with no `; readable-name` comment beside it, which
//  extract_gen.py needs to resolve the operand - so the generated side
//  printed a bare symbol where the target printed `dword_6FAB3C08`, and
//  this function fell from 4/4 EXACT to 3/4 the moment the global was
//  given a funcmap DATA entry (which it needs, or the .mix does not
//  link).  The C++ spelling keeps both sides resolving to the same name.
//  build_mix.py binds it either way: it keys extern_map on the readable
//  name, not on the decoration.
extern int dword_6FAB3C08[];
int __fastcall LookupAbilityTableEntry(int row, int col)
{
    return dword_6FAB3C08[row + col * 6];
}

//  0x6F2BB570 - RENAMED and MOVED to Missile/bulletbase_dispatch.cpp this
//  session: it is CBulletBase's own vtable slot 45
//  (CBulletBase::InvokeSlot0xBC), not an unidentified "SVTableSlot0xBCOwner"
//  placeholder - see Missile/bulletbase.h.

//  Five more bare FourCC constants - depth-1 callees pulled in off slot
//  107's own CUnit vtable closure (cunit_vtable_closure_worklist.json,
//  batch agent-cunit-batch-3).  Each is the identical
//  `mov eax, <imm32> / retn` shape as the two above; none has any other
//  instruction, so there is nothing to guess at.
unsigned int GetFourCC_6F110BC0()
{
    return 0x416D6274;
}
unsigned int GetFourCC_6F0260D0()
{
    return 0x41777270;
}
unsigned int GetFourCC_6F027BD0()
{
    return 0x4152616C;
}
unsigned int GetFourCC_6F0824F0()
{
    return 0x41676C64;
}
unsigned int GetFourCC_6F0DB030()
{
    return 0x41496E76;
}

//  Five more bare FourCC constants - depth-2 callees off CUnit slot 107's
//  own closure (cunit_vtable_closure_worklist.json, batch agent-cunit-
//  batch-17), pulled fresh via ida_query.py since none had a captured dump
//  before this session.  Identical `mov eax, <imm32> / retn` shape as
//  every other entry in this run.
unsigned int GetFourCC_6F0226A0()
{
    return 0x41616C72;
}
unsigned int GetFourCC_6F024910()
{
    return 0x41626474;
}
unsigned int GetFourCC_6F024F00()
{
    return 0x41687272;
}
unsigned int GetFourCC_6F0278B0()
{
    return 0x41717565;
}
unsigned int GetFourCC_6F028740()
{
    return 0x4173656C;
}

//  0x6F01F590 - same shape as the FourCC getters above but returns a
//  pointer to a fixed global rather than an immediate constant (`mov eax,
//  offset unk_6FAAE788 / retn`).  Reached from CUnit slot 107's closure
//  (sub_6F01DC90 calls it with no `this`/args at all and reads back
//  `eax->m_field13C`).  `unk_6FAAE788`'s own layout is not established
//  anywhere this dump reaches, so it stays an opaque pointer.
extern char unk_6FAAE788[];
void* GetGlobalPointer_6F01F590()
{
    return unk_6FAAE788;
}

//  0x6F251C50 - same slot-107 closure.  `this+8` is a reference-counted
//  object pointer; release it through the shared release-forward
//  (0x6F4C8870, Agent/refcnt_release_forward.cpp) if non-null, else do
//  nothing.  Declared exactly as that TU defines it so the two mangled
//  names are one symbol (tools/link_check.py, cause 1).
class TRefCnt;
void __fastcall ReleaseForward_6F4C8870(TRefCnt* self);
struct SField8RefOwner { void __thiscall ReleaseField8Ref(); };
void __thiscall SField8RefOwner::ReleaseField8Ref()
{
    TRefCnt* ref = *(TRefCnt**)((char*)this + 8);
    if (ref)
        ReleaseForward_6F4C8870(ref);
}

//  0x6F2555E0 / 0x6F2555F0 - same closure.  Each answers whether the
//  fastcall argument shares any bit with a fixed global mask, boolean-ized
//  to a strict 0/1 - MSVC's own `and / neg / sbb / neg` idiom for a plain
//  `(mask & flags) != 0`.
extern unsigned int dword_6FA6F2CC;
extern unsigned int dword_6FA6F2D0;
int __fastcall TestMaskBit_6FA6F2CC(unsigned int flags)
{
    return (dword_6FA6F2CC & flags) != 0;
}
int __fastcall TestMaskBit_6FA6F2D0(unsigned int flags)
{
    return (dword_6FA6F2D0 & flags) != 0;
}

//  0x6F00C230 - same closure.  Plain `__stdcall` (no `ecx` touched at
//  all, three stack floats, `retn 0Ch`): is `fabs(a - b) < epsilon`?  The
//  shipped body computes the difference, takes its absolute value with a
//  second explicit round trip through the stack (matching the two
//  separate `fstp`/`fld` pairs rather than one `fabs` left on the FPU
//  stack), then compares against `epsilon`.
int __stdcall FloatNearlyEqual_6F00C230(float a, float b, float epsilon)
{
    float diff = a - b;
    float absDiff = fabsf(diff);
    return absDiff < epsilon;
}
