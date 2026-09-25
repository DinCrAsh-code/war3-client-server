//============================================================================
//  A four-byte packed colour, addressed a channel at a time.
//
//  Every function in this family reads and writes the four bytes
//  individually (`movzx eax, byte ptr [ebp+3]`, `mov [ecx+2], al`) but
//  compares and copies the whole thing as one dword, which is why it is a
//  struct of four chars and not an `unsigned int` with shifts: a shift
//  spelling produces `shr`/`and` pairs where the shipped code has plain
//  byte loads and stores.
//
//  Which channel is which is not recoverable from these call trees - only
//  that channel 3 is the one CWidget's and CItem's slot 65 hand to sprite
//  slot 0x34 on its own while the other three ride along in the team
//  colour - so they are numbered rather than named.
//============================================================================
#ifndef PACKEDCOLOR_H
#define PACKEDCOLOR_H

struct SPackedColor
{
    unsigned char m_c0;     // +0x00
    unsigned char m_c1;     // +0x01
    unsigned char m_c2;     // +0x02
    unsigned char m_c3;     // +0x03
};

//  0x6F266760 - per-channel modulate: out[i] = a[i] * b[i] / 255, highest
//  channel first.  See colormodulate.cpp.
SPackedColor* __fastcall ModulateColors(SPackedColor* out,
                                        const SPackedColor* a,
                                        const SPackedColor* b);

#endif
