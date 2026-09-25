//============================================================================
//  Trivial field writers - one to seven instructions each, no calls, taking
//  `this` in ecx and their value(s) on the stack (`retn 4`/`8`/`0Ch`, the
//  __thiscall shape).  Grouped one translation unit for the batch, the same
//  way trivialaccessors_*.cpp already groups its own: none of them calls
//  anything, so there is nothing here for the compiler to inline into
//  anything else and CLAUDE.md's one-TU-per-module rule has nothing to bite
//  on.
//
//  Each owner below is an opaque receiver, one per address neighbourhood -
//  the classes themselves are not established by any call tree in this repo,
//  so only the offsets each function actually touches are asserted and no
//  surrounding layout is invented.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F60xxxx / 0x6F61xxxx / 0x6F62xxxx - one owner, the "dirty flag" family:
//  most of these stamp a 1 into a second field after writing the first, which
//  is what a setter on an object with a recompute-on-demand cache looks like.
//----------------------------------------------------------------------------
struct SDirtyFlagOwner
{
    void __thiscall OrField1E0(int bits);              // 0x6F601800
    void __thiscall SetField140(int value);            // 0x6F603030
    void __thiscall SetField124And128(int a, int b);   // 0x6F603040
    void __thiscall SetField12CTriple(int a, int b, int c); // 0x6F603060
    void __thiscall OrField128And124(int bits);        // 0x6F606EA0
    void __thiscall OrField124WithField128(int bits);  // 0x6F606E60
    void __thiscall OrField1CCWithField1D0(int bits);  // 0x6F626730
    void __thiscall SetField21C(int value);            // 0x6F611390
    void __thiscall SetField220(int value);            // 0x6F611370
    void __thiscall SetField294(int value);            // 0x6F624060
    void __thiscall SetField2A0(int value);            // 0x6F624080
    void __thiscall SetField298(float value);          // 0x6F624020
    void __thiscall SetField29C(float value);          // 0x6F624040
    void __thiscall SetField170IfChanged(int value);   // 0x6F623F60
    void __thiscall SetField28CIfChanged(int value);   // 0x6F6243B0
    void __thiscall SetField1F4IfChanged(const int* value); // 0x6F611590
    void __thiscall OrIndirectField168(int bits);      // 0x6F6222F0
    void __thiscall OrIndirectField16C(int bits);      // 0x6F622350
};

//  0x6F601800 - OR into +0x1E0.
void __thiscall SDirtyFlagOwner::OrField1E0(int bits)
{
    *(int*)((char*)this + 0x1E0) |= bits;
}

//  0x6F603030 - plain store to +0x140.
void __thiscall SDirtyFlagOwner::SetField140(int value)
{
    *(int*)((char*)this + 0x140) = value;
}

//  0x6F603040 - the second argument is stored first: the shipped code loads
//  arg_4 into eax and arg_0 into edx before either store, which is the order
//  MSVC picks when the *later* field is written from the *earlier* argument.
void __thiscall SDirtyFlagOwner::SetField124And128(int a, int b)
{
    *(int*)((char*)this + 0x124) = b;
    *(int*)((char*)this + 0x128) = a;
}

//  0x6F603060 - same shape one argument wider.
void __thiscall SDirtyFlagOwner::SetField12CTriple(int a, int b, int c)
{
    *(int*)((char*)this + 0x12C) = c;
    *(int*)((char*)this + 0x130) = a;
    *(int*)((char*)this + 0x134) = b;
}

//  0x6F606EA0 - the same bits into both fields.
void __thiscall SDirtyFlagOwner::OrField128And124(int bits)
{
    *(int*)((char*)this + 0x128) |= bits;
    *(int*)((char*)this + 0x124) |= bits;
}

//  0x6F606E60 - +0x128 is *read*, not written: the value folded into +0x124
//  is the union of the two.
void __thiscall SDirtyFlagOwner::OrField124WithField128(int bits)
{
    *(int*)((char*)this + 0x124) |= *(int*)((char*)this + 0x128) | bits;
}

//  0x6F626730 - the same shape on the neighbouring pair.
void __thiscall SDirtyFlagOwner::OrField1CCWithField1D0(int bits)
{
    *(int*)((char*)this + 0x1CC) |= *(int*)((char*)this + 0x1D0) | bits;
}

//  0x6F611390 / 0x6F611370 - store, then raise the +0x224 dirty flag.
void __thiscall SDirtyFlagOwner::SetField21C(int value)
{
    *(int*)((char*)this + 0x21C) = value;
    *(int*)((char*)this + 0x224) = 1;
}

void __thiscall SDirtyFlagOwner::SetField220(int value)
{
    *(int*)((char*)this + 0x220) = value;
    *(int*)((char*)this + 0x224) = 1;
}

//  0x6F624060 / 0x6F624080 - the same against the +0x168 dirty flag.
void __thiscall SDirtyFlagOwner::SetField294(int value)
{
    *(int*)((char*)this + 0x294) = value;
    *(int*)((char*)this + 0x168) = 1;
}

void __thiscall SDirtyFlagOwner::SetField2A0(int value)
{
    *(int*)((char*)this + 0x2A0) = value;
    *(int*)((char*)this + 0x168) = 1;
}

//  0x6F624020 / 0x6F624040 - the float pair.  The dirty-flag store lands
//  *between* the `fld` and the `fstp` in the shipped code, which is just the
//  x87 store being scheduled around an independent integer store; writing the
//  flag second in the source is what produces it.
void __thiscall SDirtyFlagOwner::SetField298(float value)
{
    *(float*)((char*)this + 0x298) = value;
    *(int*)((char*)this + 0x168) = 1;
}

void __thiscall SDirtyFlagOwner::SetField29C(float value)
{
    *(float*)((char*)this + 0x29C) = value;
    *(int*)((char*)this + 0x168) = 1;
}

//  0x6F623F60 / 0x6F6243B0 - guarded setters: no store and no dirty flag when
//  the value is already there.
void __thiscall SDirtyFlagOwner::SetField170IfChanged(int value)
{
    if (*(int*)((char*)this + 0x170) != value)
    {
        *(int*)((char*)this + 0x170) = value;
        *(int*)((char*)this + 0x168) = 1;
    }
}

void __thiscall SDirtyFlagOwner::SetField28CIfChanged(int value)
{
    if (*(int*)((char*)this + 0x28C) != value)
    {
        *(int*)((char*)this + 0x28C) = value;
        *(int*)((char*)this + 0x168) = 1;
    }
}

//  0x6F611590 - the same, except the new value arrives by pointer and the
//  dirty flag is +0x23C.
void __thiscall SDirtyFlagOwner::SetField1F4IfChanged(const int* value)
{
    if (*(int*)((char*)this + 0x1F4) != *value)
    {
        *(int*)((char*)this + 0x1F4) = *value;
        *(int*)((char*)this + 0x23C) = 1;
    }
}

//  0x6F6222F0 / 0x6F622350 - the same union-OR as OrField124WithField128,
//  one indirection out: the pair lives in a sub-object the field points at.
void __thiscall SDirtyFlagOwner::OrIndirectField168(int bits)
{
    int* pair = *(int**)((char*)this + 0x168);
    pair[0] |= pair[1] | bits;
}

void __thiscall SDirtyFlagOwner::OrIndirectField16C(int bits)
{
    int* pair = *(int**)((char*)this + 0x16C);
    pair[0] |= pair[1] | bits;
    *(int*)((char*)this + 0x174) = 1;
}

//----------------------------------------------------------------------------
//  0x6F38Bxxxx / 0x6F4Fxxxx / 0x6F73Axxxx / 0x6F33Axxxx - plain stores with no
//  dirty flag, and the two exclusive-or bit setters.
//----------------------------------------------------------------------------
struct SPlainFieldOwner
{
    void __thiscall SetField1A4(int value);      // 0x6F38BF70
    void __thiscall SetField1A8(int value);      // 0x6F38BF80
    void __thiscall SetField190(int value);      // 0x6F4F1FD0
    void __thiscall SetField08(int value);       // 0x6F4F4A70
    void __thiscall SetField04(int value);       // 0x6F73AED0
    void __thiscall SetField638(int value);      // 0x6F33AC00
    void __thiscall SetField194Bit0(int on);     // 0x6F4F1EA0
    void __thiscall SetField194Bit4(int on);     // 0x6F4F21C0
};

void __thiscall SPlainFieldOwner::SetField1A4(int value)
{
    *(int*)((char*)this + 0x1A4) = value;
}

void __thiscall SPlainFieldOwner::SetField1A8(int value)
{
    *(int*)((char*)this + 0x1A8) = value;
}

void __thiscall SPlainFieldOwner::SetField190(int value)
{
    *(int*)((char*)this + 0x190) = value;
}

void __thiscall SPlainFieldOwner::SetField08(int value)
{
    *(int*)((char*)this + 0x08) = value;
}

void __thiscall SPlainFieldOwner::SetField04(int value)
{
    *(int*)((char*)this + 0x04) = value;
}

//  0x6F33AC00 - one store and two flags, both set from the same 1 the shipped
//  code materialises once into eax.
void __thiscall SPlainFieldOwner::SetField638(int value)
{
    *(int*)((char*)this + 0x638) = value;
    *(int*)((char*)this + 0x620) = 1;
    *(int*)((char*)this + 0x61C) = 1;
}

//  0x6F4F1EA0 / 0x6F4F21C0 - both write a single bit of the dword at +0x194,
//  and both do it with MSVC's own bitfield-assignment sequence: load the
//  word, xor in the (shifted) new value, mask to the one bit, and fold that
//  difference back with `xor [mem], reg` - a read-modify-write, not a
//  recomputed store.
//
//  Writing that sequence out by hand as `*f ^= (*f ^ on) & mask` does *not*
//  reproduce it: the compiler common-subexpressions the two reads of `*f`
//  into a register and finishes with `mov [mem], reg`, one instruction
//  longer and a different last opcode.  Declaring the bits is what produces
//  the shipped stream, which is also evidence about the original: this field
//  is a bitfield in the shipped source, not an int with hand-written masks.
struct SField194Bits
{
    unsigned int m_bit0 : 1;
    unsigned int m_bits1to3 : 3;
    unsigned int m_bit4 : 1;
    unsigned int m_rest : 27;
};

void __thiscall SPlainFieldOwner::SetField194Bit0(int on)
{
    ((SField194Bits*)((char*)this + 0x194))->m_bit0 = on;
}

void __thiscall SPlainFieldOwner::SetField194Bit4(int on)
{
    ((SField194Bits*)((char*)this + 0x194))->m_bit4 = on;
}

//----------------------------------------------------------------------------
//  0x6F654xxx / 0x6F6CExxx - writers that reach through their arguments.
//----------------------------------------------------------------------------
struct SOutParamOwner
{
    void __thiscall ClearTwoOutputs(int* a, int b, int* c);      // 0x6F654230
    void __thiscall ReadPairInto(int* a, int b, int* c);         // 0x6F654210
    void __thiscall SetIndexedPair(int index, int a, int b);     // 0x6F6CEF40
};

//  0x6F654230 - the middle argument is never read; both outputs are zeroed.
void __thiscall SOutParamOwner::ClearTwoOutputs(int* a, int, int* c)
{
    *a = 0;
    *c = 0;
}

//  0x6F654210 - and the same pair of outputs filled from +0x18/+0x1C.
void __thiscall SOutParamOwner::ReadPairInto(int* a, int, int* c)
{
    *a = *(int*)((char*)this + 0x18);
    *c = *(int*)((char*)this + 0x1C);
}

//  0x6F6CEF40 - two parallel arrays at +0x244 and +0x254, one index.
void __thiscall SOutParamOwner::SetIndexedPair(int index, int a, int b)
{
    *(int*)((char*)this + index * 4 + 0x244) = a;
    *(int*)((char*)this + index * 4 + 0x254) = b;
}

//----------------------------------------------------------------------------
//  0x6F513950 - `this` in ecx, the first value in edx and the second on the
//  stack: a genuine __fastcall free function, not a __thiscall member (a
//  member's first argument would be on the stack and it would clean eight
//  bytes, not four).
//----------------------------------------------------------------------------
void __fastcall SetPairIfFlagged_6F513950(void* self, int a, int b)
{
    if (*(unsigned char*)((char*)self + 0x54) & 0x80)
    {
        *(int*)((char*)self + 0xCC) = a;
        *(int*)((char*)self + 0xD0) = b;
    }
}
