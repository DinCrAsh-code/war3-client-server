//============================================================================
//  SModelProbeElementA/B - the two probe-array element types
//  SModelInstance_PropagateFlagRecursive_6F507CE0 (modelinstancenotify.cpp)
//  walks.  Split into their own header, with each `ApplyFlag_*` body in its
//  own translation unit (modelprobeelementa_applyflag.cpp /
//  modelprobeelementb_applyflag.cpp): both are real, tiny, non-naked bodies
//  now, and the shipped module never inlined them into the 0x6F507CE0 walk -
//  a single-TU declaration would let /Ob2 do exactly that here and change
//  0x6F507CE0's own compiled shape (measured: -22 instructions matched).
//============================================================================
#ifndef MODELPROBEELEMENT_H
#define MODELPROBEELEMENT_H

//----------------------------------------------------------------------------
//  0x6F4F4A80 - a plain thiscall member with one stack argument (`retn 4`,
//  confirmed by its own dump, sub_6F4F4A80_0x6F4F4A80_calltree_asm.md):
//  stores `flag` straight into +0xC of the element, no gating, no read.
//  Only the touched member is named; the rest of the 0x68-stride element is
//  unestablished (see SModelInstanceProbe's own note in
//  modelinstancenotify.cpp).
//----------------------------------------------------------------------------
struct SModelProbeElementA
{
    char m_reserved00[0xC];
    int  m_flag0C;      // +0xC

    void __thiscall ApplyFlag_6F4F4A80(int flag);
};

//----------------------------------------------------------------------------
//  0x6F4F1EC0 - a plain thiscall member with one stack argument (`retn 4`,
//  confirmed by its own dump).  Sets bit 1 of +0x194 to `flag`'s own bit 0,
//  branchlessly (`edx = (flag<<1) ^ field; edx &= 2; field ^= edx`) - the
//  xor/and/xor "assign one bit, leave the rest" shape
//  docs/msvc-vc8-idioms.md's "A bit further in than bit 0" note documents
//  for this exact +0x194 field family: declaring the bitfield is what
//  reproduces the shift-then-mask-then-store-back instead of a longer
//  read/xor/and/xor/store sequence.
//----------------------------------------------------------------------------
struct SModelProbeElementBFlags194
{
    unsigned int m_bit0 : 1;
    unsigned int m_isFlagged : 1;    // +0x194, bit 1 - what ApplyFlag_6F4F1EC0 sets
    unsigned int m_rest : 30;
};

struct SModelProbeElementB
{
    char m_reserved00[0x194];
    SModelProbeElementBFlags194 m_flags194;   // +0x194

    void __thiscall ApplyFlag_6F4F1EC0(int flag);
};

#endif
