//============================================================================
//  0x6F40B2B0 / 0x6F40B2C0 / 0x6F40B2D0 / 0x6F40B2E0 / 0x6F40B2F0 - five
//  one-instruction score accessors on the player record.
//
//  Four are plain loads and the fifth hands back the *address* of the
//  record at +0x2E4 (`lea eax, [ecx+2E4h]`), not its contents - which is
//  why it returns a pointer and the other four do not.  All five are
//  `retn 0` with `this` in ecx.
//
//  One translation unit for the five: none of them calls another, they are
//  four bytes apart in the shipped image, and the only thing that matters
//  for the match is that each keeps its own address.
//============================================================================
#include "playerscore.h"

unsigned int SPlayerScore::QueryPeak24C()    { return m_peak24C; }
unsigned int SPlayerScore::QueryCounter250() { return m_counter250; }
unsigned int SPlayerScore::QueryCounter254() { return m_counter254; }
unsigned int SPlayerScore::QueryCounter258() { return m_counter258; }
void*        SPlayerScore::GetRecord2E4()    { return m_record2E4; }
