//============================================================================
//  The 0x5C-byte accumulation block the hero-value terms fill and read.
//
//  Three ints and eleven CFloats, in three groups the initialiser
//  (0x6F077E80) treats alike - each group's first word is a mode of 2 - plus
//  a run of nine more CFloats and three plain int totals at the end.  Only
//  the shape is claimed: nothing in this call tree says what any individual
//  field means, and the three the hero terms actually read back are the
//  three ints at +0x50/+0x54/+0x58.
//============================================================================
#ifndef UNITVALUEBLOCK_H
#define UNITVALUEBLOCK_H

#include "CFloat.h"

struct SUnitValueBlock
{
    //  Every local of this type is filled by Init() before anything reads
    //  it, so the eleven CFloat members must NOT run their own zeroing
    //  default constructor first - eleven dead stores per local, and the
    //  shipped code has none of them.  The tagged constructor compiles to
    //  nothing (CFloat.h).
    SUnitValueBlock(ECFloatNoInit)
        : m_f10(kCFloatNoInit), m_f14(kCFloatNoInit), m_f1C(kCFloatNoInit),
          m_f20(kCFloatNoInit), m_f24(kCFloatNoInit), m_f28(kCFloatNoInit),
          m_f2C(kCFloatNoInit), m_f30(kCFloatNoInit), m_f34(kCFloatNoInit),
          m_f38(kCFloatNoInit), m_f3C(kCFloatNoInit), m_f40(kCFloatNoInit),
          m_f44(kCFloatNoInit), m_f48(kCFloatNoInit), m_f4C(kCFloatNoInit) {}

    //  0x6F077E80 - put the block back to its starting state and hand it
    //  back.  It really does return `this`: the shipped body opens
    //  `mov eax, ecx` and addresses everything through eax, so the pointer
    //  is in the return register on every path by construction.
    SUnitValueBlock* Init();

    int    m_mode00;   // +0x00 - 2
    int    m_count04;  // +0x04
    int    m_count08;  // +0x08
    int    m_mode0C;   // +0x0C - 2
    CFloat m_f10;      // +0x10
    CFloat m_f14;      // +0x14
    int    m_mode18;   // +0x18 - 2
    CFloat m_f1C;      // +0x1C
    CFloat m_f20;      // +0x20
    CFloat m_f24;      // +0x24
    CFloat m_f28;      // +0x28
    CFloat m_f2C;      // +0x2C
    CFloat m_f30;      // +0x30
    CFloat m_f34;      // +0x34
    CFloat m_f38;      // +0x38
    CFloat m_f3C;      // +0x3C
    CFloat m_f40;      // +0x40
    CFloat m_f44;      // +0x44
    CFloat m_f48;      // +0x48
    CFloat m_f4C;      // +0x4C
    int    m_total50;  // +0x50
    int    m_total54;  // +0x54
    int    m_total58;  // +0x58
};

//  0x6F077FA0 - walk the chain of contributors hanging off the unit's
//  +0x1DC handle pair, asking each one for its own pair of blocks and
//  adding them into the caller's two.  `retn 8`, `this` in ecx.
struct SUnitValueSource
{
    void Accumulate(SUnitValueBlock* a, SUnitValueBlock* b);
};

#endif
