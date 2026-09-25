//============================================================================
//  A further batch of small, self-contained leaves reached from the spell/
//  agent-message closures.  None of these has its owning class otherwise
//  identified in this closure; each is modelled as a bare-offset operation
//  on an opaque `this`.
//============================================================================
#include "CFloat.h"

//  0x6F4B2E90 - convert three floats (from an input triple pointed to by
//  the stack argument) to ints via the CRT's own __ftol2_sse, store them
//  into this+0/+4/+8, return `this`.
struct SVec3IntConverter { void* __thiscall ConvertFloatTripleToInt(const float* src); };
void* __thiscall SVec3IntConverter::ConvertFloatTripleToInt(const float* src)
{
    int* dst = (int*)this;
    dst[0] = (int)src[0];
    dst[1] = (int)src[1];
    dst[2] = (int)src[2];
    return this;
}

//  0x6F2556E0 - build a 9-dword struct from five stack arguments plus
//  g_CFloatZero and two zeroed trailing words.
struct SPacked9DwordBuilder
{
    void __thiscall Build(void* a, void* b, void* c, void* d, void* e);
};
void __thiscall SPacked9DwordBuilder::Build(void* a, void* b, void* c, void* d, void* e)
{
    void** fields = (void**)this;
    fields[4] = a;      // +0x10
    fields[5] = b;      // +0x14
    fields[0] = c;      // +0x00
    fields[1] = d;      // +0x04
    fields[3] = e;      // +0x0C
    fields[6] = (void*)&g_CFloatZero; // +0x18
    fields[7] = 0;      // +0x1C
    fields[8] = 0;      // +0x20
}

//  0x6F741AC0 / 0x6F741B00 / 0x6F741B90 - identical shape, three different
//  bit masks (2, 0x20, 0x100): if the stack flag is *set*, set the bit on
//  this+0x2CC's own indexed array element (+4), else clear it - either way
//  return the element's address.  From the same 0x6F741xxx module as
//  agenteventrouter.h's sibling handlers (own class not identified).
//
//  The polarity used to be written the other way round here (the parameter
//  was called `clearFlag` and non-zero meant clear).  That is backwards:
//  the shipped `cmp [esp+arg_4],0` / `jz` takes the *zero* case to the
//  `and ...,0FFFFFFFDh` clear branch and falls through to the `or ...,2`
//  set branch, and the call sites agree - configgates.cpp's
//  AttachmentSetVisible(slot, 1) reaches SetOrClearBit0x20 to make an
//  attachment visible, not to hide it.  Fixing it is what takes all three
//  from 7/15 to EXACT.
//
//  The element address is recomputed inside each branch rather than once
//  ahead of the test: the shipped code emits `mov ecx,[ecx+2CCh]` /
//  `lea`/`shl` in the set branch and a second, differently allocated copy
//  (`lea edx` / `mov eax,[ecx+2CCh]` / `shl edx`) in the clear branch.  One
//  shared local ahead of the `if` hoists both out and costs eight of the
//  fifteen instructions.
struct SIndexedArrayFlags
{
    void* __thiscall SetOrClearBit2(int index, int setFlag);
    void* __thiscall SetOrClearBit0x20(int index, int setFlag);
    void* __thiscall SetOrClearBit0x100(int index, int setFlag);
};

void* __thiscall SIndexedArrayFlags::SetOrClearBit2(int index, int setFlag)
{
    if (setFlag)
    {
        int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
        *slot |= 2;
        return slot;
    }
    int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
    *slot &= ~2;
    return slot;
}

void* __thiscall SIndexedArrayFlags::SetOrClearBit0x20(int index, int setFlag)
{
    if (setFlag)
    {
        int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
        *slot |= 0x20;
        return slot;
    }
    int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
    *slot &= ~0x20;
    return slot;
}

void* __thiscall SIndexedArrayFlags::SetOrClearBit0x100(int index, int setFlag)
{
    if (setFlag)
    {
        int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
        *slot |= 0x100;
        return slot;
    }
    int* slot = (int*)(*(char**)((char*)this + 0x2CC) + (index * 0xA0) + 4);
    *slot &= ~0x100;
    return slot;
}

//  0x6F7418E0 - write a two-dword pair (from a pointer arg) into this+0x2CC's
//  own indexed array element (+0x18/+0x1C), then OR bit 8 into +4 and
//  return the element's address.
struct SIndexedArrayWriter { void* __thiscall WritePairAndFlag(int index, const void** pair); };
void* __thiscall SIndexedArrayWriter::WritePairAndFlag(int index, const void** pair)
{
    char* base = *(char**)((char*)this + 0x2CC);
    int offset = index * 0xA0;
    *(const void**)(base + offset + 0x18) = pair[0];
    *(const void**)(base + offset + 0x1C) = pair[1];
    base = *(char**)((char*)this + 0x2CC);
    *(int*)(base + offset + 4) |= 8;
    return base + offset + 4;
}

//  0x6F4D3E60 - decrement a byte counter at this+0; on reaching zero, reset
//  it and a second byte at this+1/+2 to 0.  Otherwise recompute the byte at
//  this+2 as `(this+2's old value + this+8's dword - 1) % this+8's dword`.
struct SRoundRobinCounter { void __thiscall Advance(); };
void __thiscall SRoundRobinCounter::Advance()
{
    unsigned char* counter = (unsigned char*)this;
    *counter -= 1;
    if (*counter != 0)
    {
        unsigned int period = *(unsigned int*)((char*)this + 8);
        unsigned int index = (unsigned char)((char*)this)[2];
        ((char*)this)[2] = (char)((index + period - 1) % period);
        return;
    }

    ((char*)this)[2] = 0;
    ((char*)this)[1] = 0;
}

//  0x6F266720 - zero-initialise a struct's own +0x0/+0xC.. +0x38 float
//  fields, +0x4C and set +0x50 to a fixed constant (0xE).
struct SZeroInitStruct { void __thiscall ZeroFields(); };
void __thiscall SZeroInitStruct::ZeroFields()
{
    int* fields = (int*)this;
    fields[0] = 0;
    CFloat zero;
    *(CFloat*)((char*)this + 0x0C) = zero;
    *(CFloat*)((char*)this + 0x10) = zero;
    *(CFloat*)((char*)this + 0x14) = zero;
    *(CFloat*)((char*)this + 0x18) = zero;
    *(CFloat*)((char*)this + 0x1C) = zero;
    *(CFloat*)((char*)this + 0x20) = zero;
    *(CFloat*)((char*)this + 0x30) = zero;
    *(CFloat*)((char*)this + 0x34) = zero;
    *(CFloat*)((char*)this + 0x38) = zero;
    *(int*)((char*)this + 0x4C) = 0;
    *(int*)((char*)this + 0x50) = 0xE;
}
