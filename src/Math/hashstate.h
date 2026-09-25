//============================================================================
//  A four-rotor state advance (0x6F011390) over a 256-entry substitution
//  table.  Nothing in this call tree explains what the two 32-bit fields
//  feed or what consumes them afterwards - only the one function that
//  advances the state is reached, so it is named for what it does
//  mechanically (a rotor-cipher-shaped scrambler), not for a purpose.
//============================================================================
#ifndef HASHSTATE_H
#define HASHSTATE_H

struct SHashState
{
    //  0x6F011390 - advance the four byte-rotors packed into m_hi, mix a
    //  new value into m_lo and return it - RandomCFloat() (randomcfloat.cpp)
    //  reads the new m_lo straight out of eax rather than reloading through
    //  the pointer, so a `void` declaration here would leave it reading
    //  stale scratch instead (the same crash class CFloatTruncateFrac hit -
    //  see docs/msvc-vc8-idioms.md).
    int Advance();

    int m_lo;   // +0x00
    int m_hi;   // +0x04
};

//  dword_6F953650 - the substitution table every rotor indexes into.  The
//  shipped code addresses it `table[byteOffset]`, not `table[i*4]` - a
//  4-byte dword is read starting at a *byte* offset, so the four rotor
//  windows (sizes 0xBC/0xD4/0xEC/0xF4, each aligned to 4 in the underlying
//  bytes since every rotor value already comes from a shift-and-mask) sit
//  only one byte apart, not four.  Declared as an opaque byte array: its
//  contents are data, not code, and nothing in this call tree needs a real
//  definition for the instruction stream to match.
extern const unsigned char g_hashSubstitutionTable[];   // dword_6F953650

//  A dword read starting at byte offset `byteOffset` into the table -
//  reproduces the shipped code's `mov reg, table[byteOffset]` addressing
//  (scale 1, not the scale-4 an `unsigned int[]` index would produce).
inline unsigned int HashTableDword(int byteOffset)
{
    return *(const unsigned int*)(g_hashSubstitutionTable + byteOffset);
}

#endif
