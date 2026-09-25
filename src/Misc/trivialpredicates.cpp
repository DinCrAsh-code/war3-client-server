//============================================================================
//  Small branching accessors and predicates from across the image: each one
//  reads a couple of fields of the object in ecx (and, where the shipped
//  code uses edx, one register argument) and returns a value; none of them
//  calls anything.  Same reasoning as trivialaccessors_*.cpp for why they
//  share a translation unit and why they are free __fastcall functions
//  rather than members of reconstructed classes: nothing to inline into
//  anything, and no class identity is established for any of these
//  receivers.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F26BA80 - +0x28, unless bit 0 of the flag byte at +0x20 is set.
//----------------------------------------------------------------------------
int __fastcall GetField0x28UnlessFlag_6F26BA80(const void* self)
{
    const char* p = (const char*)self;
    //  The shipped code falls through to the field read and branches away
    //  to the zero, so the field read is the first return here - source
    //  order is what fixes the block layout, not the test's polarity.
    if ((*(const unsigned char*)(p + 0x20) & 1) == 0)
        return *(const int*)(p + 0x28);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2F41E0 - follow the pointer at +0x3C8 and read its +0x15C.
//----------------------------------------------------------------------------
int __fastcall GetIndirectField0x15C_6F2F41E0(const void* self)
{
    const char* p = *(const char* const*)((const char*)self + 0x3C8);
    if (p != 0)
        return *(const int*)(p + 0x15C);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C3680 - follow +0x0C and read its own +0x0C.  The shipped guard is
//  a *signed* `jle`, not a null test, so the field is compared as an int.
//----------------------------------------------------------------------------
int __fastcall GetIndirectField0xC_6F4C3680(const void* self)
{
    int head = *(const int*)((const char*)self + 0x0C);
    if (head > 0)
        return *(const int*)((const char*)head + 0x0C);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4D3740 - address of the last element of the {count @ +4, base @ +8}
//  dword array, or 0 when the count is 0.
//----------------------------------------------------------------------------
void* __fastcall GetLastArrayElement_6F4D3740(const void* self)
{
    const char* p = (const char*)self;
    int count = *(const int*)(p + 4);
    if (count != 0)
        return (char*)*(void* const*)(p + 8) + count * 4 - 4;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4DE1F0 - bit 0 of +0x10.
//----------------------------------------------------------------------------
int __fastcall GetFlag0x10Bit0_6F4DE1F0(const void* self)
{
    return *(const int*)((const char*)self + 0x10) & 1;
}

//----------------------------------------------------------------------------
//  0x6F502880 - is the dword at +4 exactly 1?
//----------------------------------------------------------------------------
int __fastcall IsField0x4One_6F502880(const void* self)
{
    return *(const int*)((const char*)self + 4) == 1;
}

//----------------------------------------------------------------------------
//  0x6F502A00 - +0x100, but only when bit 4 of the flag byte at +0x94 is set.
//----------------------------------------------------------------------------
int __fastcall GetField0x100IfFlag_6F502A00(const void* self)
{
    const char* p = (const char*)self;
    if ((*(const unsigned char*)(p + 0x94) & 0x10) == 0)
        return 0;
    return *(const int*)(p + 0x100);
}

//----------------------------------------------------------------------------
//  0x6F50F3C0 - bit 0 of the flag byte at +4, normalised to 0/1.
//----------------------------------------------------------------------------
int __fastcall TestFlag0x4Bit0_6F50F3C0(const void* self)
{
    //  Written as a zeroed local plus a conditional store, not as a
    //  ternary or a comparison: the shipped code is `xor eax,eax` / `test`
    //  / `je` / `mov eax,1`, which is what this spelling lowers to.  A
    //  ternary lowers to movzx/and instead - same answer, four fewer
    //  matching instructions.
    int result = 0;
    if (*(const unsigned char*)((const char*)self + 4) & 1)
        result = 1;
    return result;
}

//----------------------------------------------------------------------------
//  0x6F513930 - follow +0x44 and read the byte at its +0xB8.
//----------------------------------------------------------------------------
int __fastcall GetIndirectByte0xB8_6F513930(const void* self)
{
    const char* p = *(const char* const*)((const char*)self + 0x44);
    return *(const unsigned char*)(p + 0xB8);
}

//----------------------------------------------------------------------------
//  0x6F51A740 - index the dword array at +0x28.  The index arrives in edx,
//  i.e. as __fastcall's second register parameter.
//----------------------------------------------------------------------------
int __fastcall GetArray0x28Element_6F51A740(const void* self, int index)
{
    return (*(const int* const*)((const char*)self + 0x28))[index];
}

//----------------------------------------------------------------------------
//  0x6F537790 - 0 when the tag at +0x2288 is "POOL" or "ENON" (both read
//  most-significant byte first), 1 otherwise.
//----------------------------------------------------------------------------
int __fastcall IsUnknownTag_6F537790(const void* self)
{
    unsigned int tag = *(const unsigned int*)((const char*)self + 0x2288);
    if (tag == 0x4C4F4F50u || tag == 0x4E4F4E45u)
        return 0;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F537B20 - address of whichever of the two sub-objects is currently
//  selected, or 0 when neither discriminator matches.
//----------------------------------------------------------------------------
void* __fastcall GetSelectedSubObject_6F537B20(void* self)
{
    char* p = (char*)self;
    if (*(const int*)(p + 0xBE4) == 1)
        return p + 0xBE8;
    if (*(const int*)(p + 0x614) == 3)
        return p + 0x618;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F60EDE0 - the first dword of whatever +0x178 points at, gated on the
//  count at +0x174 being non-zero.
//----------------------------------------------------------------------------
int __fastcall GetIndirectHead_6F60EDE0(const void* self)
{
    const char* p = (const char*)self;
    if (*(const int*)(p + 0x174) != 0)
        return **(const int* const*)(p + 0x178);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F62D8B0 - bit 1 of +0x44.
//----------------------------------------------------------------------------
int __fastcall GetFlag0x44Bit1_6F62D8B0(const void* self)
{
    return (*(const unsigned int*)((const char*)self + 0x44) >> 1) & 1;
}

//----------------------------------------------------------------------------
//  0x6F740930 - bit 9 of +0x970.
//----------------------------------------------------------------------------
int __fastcall GetFlag0x970Bit9_6F740930(const void* self)
{
    return (*(const unsigned int*)((const char*)self + 0x970) >> 9) & 1;
}

//----------------------------------------------------------------------------
//  0x6F650680 - zero the first four dwords, tolerating a null receiver.
//----------------------------------------------------------------------------
void __fastcall ClearFourDwords_6F650680(void* self)
{
    //  The two-instruction difference this scores against the dump is not
    //  source-addressable - see its BEHAVIOUR entry.  Tried as an early
    //  `return`, as an `int*` parameter, and as this block form; all three
    //  reuse the already-zeroed eax as the null comparand.
    if (self != 0)
    {
        *(int*)((char*)self + 0x00) = 0;
        *(int*)((char*)self + 0x04) = 0;
        *(int*)((char*)self + 0x08) = 0;
        *(int*)((char*)self + 0x0C) = 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F6CE6C0 - zero the four dwords at +0x244..+0x250.
//----------------------------------------------------------------------------
void __fastcall ClearFields0x244_6F6CE6C0(void* self)
{
    char* p = (char*)self;
    *(int*)(p + 0x244) = 0;
    *(int*)(p + 0x248) = 0;
    *(int*)(p + 0x24C) = 0;
    *(int*)(p + 0x250) = 0;
}

//----------------------------------------------------------------------------
//  0x6F664B90 - population count, by Kernighan's `x &= x - 1` loop.  The
//  value arrives in ecx.
//----------------------------------------------------------------------------
int __fastcall PopCount_6F664B90(unsigned int value)
{
    //  A guarded do/while, not a plain while: the shipped loop tests the
    //  entry condition once up front and then relies on the `and`'s own
    //  flags to close the loop.  A `while` makes this compiler re-test
    //  `value` after the `and` instead, which costs two instructions.
    int count = 0;
    if (value != 0)
    {
        do
        {
            count += 1;
            value &= value - 1;
        }
        while (value != 0);
    }
    return count;
}

//----------------------------------------------------------------------------
//  0x6F6C8740 - copy a three-float vector, negating the third component.
//  Destination in ecx, source in edx; returns the destination.
//----------------------------------------------------------------------------
float* __fastcall CopyVec3NegateZ_6F6C8740(float* dst, const float* src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = -src[2];
    return dst;
}

//----------------------------------------------------------------------------
//  0x6F3873F0 - the sum of the two floats at +0x234 and +0x230, in that
//  load order (the shipped code loads +0x234 first and adds +0x230 to it).
//----------------------------------------------------------------------------
float __fastcall SumFloats0x234And0x230_6F3873F0(const void* self)
{
    const char* p = (const char*)self;
    return *(const float*)(p + 0x234) + *(const float*)(p + 0x230);
}

//----------------------------------------------------------------------------
//  0x6F7BCC00 - remaining room: the limit at +0x34 of whatever +0x1C points
//  at (or 0xFF when +0x1C holds no positive pointer) minus the used count
//  at +0x38.  Guarded with a signed `jle`, same as 0x6F4C3680.
//----------------------------------------------------------------------------
int __fastcall GetRemaining_6F7BCC00(const void* self)
{
    const char* p = (const char*)self;
    int head = *(const int*)(p + 0x1C);
    if (head > 0)
        return *(const int*)((const char*)head + 0x34) - *(const int*)(p + 0x38) - 1;
    return 0xFF - *(const int*)(p + 0x38);
}

//----------------------------------------------------------------------------
//  0x6F7CA890 - walk the singly-linked list whose head is at +0, comparing
//  each node's +8 against the key in edx, and return the matching node.
//----------------------------------------------------------------------------
void* __fastcall FindNodeByKey_6F7CA890(const void* self, int key)
{
    char* node = *(char* const*)self;
    while (node != 0)
    {
        if (*(const int*)(node + 8) == key)
            return node;
        node = *(char**)(node + 4);
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F514820 - publish the byte at +0x58 through the out-pointer in edx,
//  but only when the object at +0x44 has a non-zero +0x1C.  Returns 1 when
//  it wrote, 0 when it did not.
//----------------------------------------------------------------------------
int __fastcall QueryByte0x58_6F514820(const void* self, int* out)
{
    const char* p = (const char*)self;
    const char* owner = *(const char* const*)(p + 0x44);
    if (*(const int*)(owner + 0x1C) == 0)
        return 0;
    *out = *(const unsigned char*)(p + 0x58);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4F21F0 - bump the reference count at +4 and hand `this` back.
//----------------------------------------------------------------------------
void* __fastcall AddRef_6F4F21F0(void* self)
{
    *(int*)((char*)self + 4) += 1;
    return self;
}
