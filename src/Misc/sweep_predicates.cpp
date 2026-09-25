//============================================================================
//  Small predicates, constant returns, global reads and global-table lookups.
//  None of them calls anything; grouped one TU for the batch as
//  trivialaccessors_*.cpp already does.
//============================================================================

//  dword_6FA4F6E8 is g_rowTableCurrentRow (rowtable.cpp), already an int
//  there - the same spelling and the same type, or the linker sees two
//  different symbols.
extern int   g_rowTableCurrentRow;      // 0x6FA4F6E8
extern void* g_unk6FAD1CA0;             // 0x6FAD1CA0
extern void* off_6FA9FCB0;              // 0x6FA9FCB0 - the fallback object
extern unsigned int g_unk6FAAC960;      // 0x6FAAC960
extern int   g_unk6FADBF4C;             // 0x6FADBF4C
extern void* g_unk6FADBC80;             // 0x6FADBC80
extern void* g_unk6FADBC84;             // 0x6FADBC84
extern int   g_unk6FADBDB0[];           // 0x6FADBDB0
extern float g_unk6F985AD0[];           // 0x6F985AD0
extern int   g_unk6F97B6EC[];           // 0x6F97B6EC

//----------------------------------------------------------------------------
//  0x6F2F4100 / 0x6F2F4200 - two members of the same object: a constant `true`
//  and an identity test against +0x1B4.  Both clean four bytes, i.e. one stack
//  argument on top of `this` in ecx.
//----------------------------------------------------------------------------
struct SIdentityOwner
{
    int __thiscall AlwaysTrue_6F2F4100(void* candidate);
    int __thiscall IsField1B4_6F2F4200(void* candidate);
};

//  0x6F2F4100 - the argument is never read.
int __thiscall SIdentityOwner::AlwaysTrue_6F2F4100(void*)
{
    return 1;
}

//  0x6F2F4200 - `setz al` on a zeroed eax, so the result is a real 0/1 and
//  not the raw difference.
int __thiscall SIdentityOwner::IsField1B4_6F2F4200(void* candidate)
{
    return *(void**)((char*)this + 0x1B4) == candidate;
}

//----------------------------------------------------------------------------
//  0x6F002F00 - a constant `false` that cleans eight bytes: two arguments,
//  neither read.  A stub override rather than a computation.
//----------------------------------------------------------------------------
struct SAlwaysFalseOwner { int __thiscall Reject(void* a, void* b); };
int __thiscall SAlwaysFalseOwner::Reject(void*, void*)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F26B7F0 - plain getter, +0x28, no stack argument.
//----------------------------------------------------------------------------
struct SField28Owner { int __thiscall GetField28(); };
int __thiscall SField28Owner::GetField28()
{
    return *(int*)((char*)this + 0x28);
}

//----------------------------------------------------------------------------
//  0x6F41FF40 - "the argument's own field, or mine": the null check picks
//  between the candidate's +0x04 and the receiver's +0x08.
//----------------------------------------------------------------------------
struct SFallbackFieldOwner { int __thiscall Field04Or08(const int* candidate); };
int __thiscall SFallbackFieldOwner::Field04Or08(const int* candidate)
{
    if (candidate)
        return candidate[1];

    return *(int*)((char*)this + 8);
}

//----------------------------------------------------------------------------
//  0x6F7A28C0 - a bare comparison against a constant with no receiver at all
//  (ecx is never touched), so __stdcall rather than __thiscall.
//----------------------------------------------------------------------------
int __stdcall IsCode0x153_6F7A28C0(int code)
{
    return code == 0x153;
}

//----------------------------------------------------------------------------
//  0x6F537EF0 - the build's own four-character version tag, chosen off
//  g_rowTableCurrentRow: 'W3XP' for expansion, 'WAR3' otherwise.  The
//  constants are byte-reversed in the dump because x86 stores them
//  little-endian; spelled here as the FourCC literals this codebase's own
//  naming scheme uses (docs/msvc-vc8-idioms.md).
//
//  The default is loaded first and the branch overwrites it, which is what
//  `eax = A; if (cond) eax = B;` compiles to - not what an if/else does.
//----------------------------------------------------------------------------
unsigned int GetProductTag_6F537EF0()
{
    unsigned int tag = 0x57415233;   // 'WAR3'

    if (g_rowTableCurrentRow == 1)
        tag = 0x57335850;            // 'W3XP'

    return tag;
}

//----------------------------------------------------------------------------
//  0x6F6CF030 - a global with a compiled-in fallback: hand back the global
//  when it is set, otherwise the address of a fixed object.
//----------------------------------------------------------------------------
void* GetGlobalOrDefault_6F6CF030()
{
    void* value = g_unk6FAD1CA0;

    if (!value)
        value = &off_6FA9FCB0;

    return value;
}

//----------------------------------------------------------------------------
//  0x6F7B4400 - store and clamp: the unclamped value is written first and
//  only overwritten when it is out of range, which is the store-then-clamp
//  order the dump has (and not what `min(x,8)` compiles to).  The bound is
//  `jb`, not `jl`, so the comparison is unsigned and the parameter is too.
//----------------------------------------------------------------------------
void __fastcall SetClampedTo8_6F7B4400(unsigned int value)
{
    g_unk6FAAC960 = value;

    if (value >= 8)
        g_unk6FAAC960 = 8;
}

//----------------------------------------------------------------------------
//  0x6F7B43D0 - bump a global counter and the receiver's own +0x24 by the
//  same one.
//
//  **Returns `this`, and that is the whole point of the signature.**  The
//  shipped body is
//
//      mov     eax, ecx          ; this -> eax, and it stays there
//      mov     ecx, 1
//      add     dword_6FADBF4C, ecx
//      add     [eax+24h], ecx
//      retn
//
//  so it leaves `this` in eax on return.  Written as a `void` member (what
//  this was until now) MSVC has no reason to move `this` anywhere: it keeps
//  it in ecx, materialises the 1 into eax instead, and the function returns
//  with **eax = 1**.  Behaviourally identical as C++ - and a live crash,
//  because a caller in this whole-program-optimised build is free to use the
//  eax the shipped function leaves behind as the object pointer.  Handing it
//  a 1 dereferences address 1.
//
//  Declaring the return type is what puts `this` back in eax; the constant
//  then has to go somewhere else and lands in ecx, which is the rest of the
//  shipped shuffle for free.  Same trick that fixed ReadSingleDword and
//  ReadHeaderDword - see their own BEHAVIOUR entries.  Nothing calls this
//  from another translation unit, so the mangled-name change is contained.
//----------------------------------------------------------------------------
struct SCountedOwner { SCountedOwner* __thiscall AddOneToBoth(); };
SCountedOwner* __thiscall SCountedOwner::AddOneToBoth()
{
    int one = 1;
    g_unk6FADBF4C += one;
    *(int*)((char*)this + 0x24) += one;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F7AD980 - park two values in a pair of adjacent globals.  __fastcall:
//  both arrive in registers and nothing is cleaned.
//----------------------------------------------------------------------------
void __fastcall StoreGlobalPair_6F7AD980(void* a, void* b)
{
    g_unk6FADBC80 = a;
    g_unk6FADBC84 = b;
}

//----------------------------------------------------------------------------
//  0x6F7AE900 / 0x6F7AE920 / 0x6F6F4AD0 - flat-table lookups on an index in
//  ecx.  The third one's `lea eax,[ecx+ecx*2]` is the table's own three-dword
//  stride, applied before the dword scale rather than folded into it.
//----------------------------------------------------------------------------
int __fastcall GetTableInt_6F7AE900(int index)
{
    return g_unk6FADBDB0[index];
}

float __fastcall GetTableFloat_6F7AE920(int index)
{
    return g_unk6F985AD0[index];
}

int __fastcall GetStride3TableInt_6F6F4AD0(int index)
{
    return g_unk6F97B6EC[index * 3];
}
