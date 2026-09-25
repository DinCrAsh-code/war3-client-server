//============================================================================
//  CMemBlock (0x6F4Bxxxx): a Storm-backed byte block that keeps a fixed-size
//  header in front of the bytes it hands out, plus the FillBytes helper it
//  uses to zero itself.
//============================================================================
#ifndef CELLBUFFER_H
#define CELLBUFFER_H

#include "storm.h"

void __fastcall FillBytes(void* dst, unsigned char val, unsigned int count);
void __fastcall FillBytesAligned(void* dst, unsigned char val,
                                 unsigned int count);
void __fastcall FillDwordGroups(void* dst, unsigned int val,
                                unsigned int byteCount);

//----------------------------------------------------------------------------
//  0x6F4B34E0/0x6F4B34F0 - the __FILE__/__LINE__ Storm's allocator wants,
//  hard-coded rather than derived from anything: CMemBlock's own source file
//  and a fixed line number, exactly like TSGrowableArray's RTTI-name tag.
//  Their own translation unit so that SetSize reaches them by call.
//----------------------------------------------------------------------------
const char* CMemBlockDebugFile();
int CMemBlockDebugLine();

//----------------------------------------------------------------------------
//  0x6F4Bxxxx - a growable byte block.
//
//  m_block is the raw Storm allocation (-1, not 0, when there is none),
//  m_blockSize its total size and m_size the size the owner asked for.  The
//  difference between the two is a header the block keeps in front of the
//  usable bytes and preserves across every resize; m_data points past it.
//  Only the five fields this call tree touches are named.
//----------------------------------------------------------------------------
class CMemBlock
{
public:
    //  0x6F4B39C0 - resize the usable area to `newSize` bytes, keeping the
    //  header, then zero the whole usable area unless `noZero`.  Always
    //  returns 1.
    int SetSize(unsigned int newSize, int noZero);

    //  0x6F4B3730 - release the raw allocation (if any) and zero every
    //  field SetSize touches.  Unlike SetSize's own "release" path (which
    //  parks m_block at the -1 sentinel), this leaves it at a plain 0 -
    //  the two call sites reaching this call tree never resize a block
    //  FreeBlock() has already run on, so nothing here has to tell "freed"
    //  apart from "never allocated".
    void FreeBlock();

    //  0x6F4B3860 - free any existing allocation, then allocate `size` +
    //  `header` fresh bytes and set up the block to hand out `size` bytes
    //  starting `header` bytes in - the same header/data split SetSize
    //  keeps, established directly instead of grown into.  A `size` of -1
    //  (checked as `size+header != 0`... the shipped code tests the sum,
    //  not either half) skips the allocation and parks m_block at -1.  Two
    //  more stack arguments arrive here than this call tree's one caller
    //  supplies values for; the shipped code never reads them either, so
    //  they stay unread parameters rather than an invented use.
    void Init(unsigned int size, unsigned int header,
             unsigned int unused1, unsigned int unused2);

    unsigned int m_reserved00;  // +0x00 (unknown - not read by this call tree)
    void*        m_block;       // +0x04
    unsigned int m_blockSize;   // +0x08
    void*        m_data;        // +0x0C  m_block + header
    unsigned int m_size;        // +0x10
};

//  0x6F4B3770 - see cellbuffer.cpp's own comment. `this` in ecx, no stack
//  args (`retn` with zero stack bytes). `__declspec(naked)` is only legal
//  on the definition, not a declaration.
void __fastcall ReleaseCMemBlockBase(void* pThis);

#endif
