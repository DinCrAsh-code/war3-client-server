//============================================================================
//  0x6F011390 - SHashState::Advance().  Four byte "rotors" packed into
//  m_hi (bits 0-7, 8-15, 16-23, 24-31), each stepped by a different
//  fixed decrement with its own wraparound modulus (0xBC, 0xD4, 0xEC,
//  0xF4), each substituted through the same 256-entry table, three of
//  the four rotated and xored together and added into m_lo, and all four
//  new positions repacked back into m_hi in the shipped byte order
//  (rotor3, rotor2, rotor1, rotor0).
//============================================================================
#include "hashstate.h"

int SHashState::Advance()
{
    int lo = m_lo;
    int hi = m_hi;

    //  Declaration order picks the rotor byte extracted from `hi` before
    //  any adjustment - the shipped code reads all four out of the same
    //  original value before touching any of them.
    int rotor0 = hi & 0xFF;
    int rotor1 = (hi >> 8) & 0xFF;
    int rotor2 = (hi >> 0x10) & 0xFF;
    int rotor3 = (unsigned int)hi >> 0x18;

    rotor2 -= 0xC;
    rotor3 -= 4;
    if (rotor3 < 0)
        rotor3 += 0xBC;
    rotor1 -= 0x18;
    if (rotor2 < 0)
        rotor2 += 0xD4;

    unsigned int sub3 = HashTableDword(rotor3);
    rotor0 -= 0x1C;
    if (rotor1 < 0)
        rotor1 += 0xEC;

    unsigned int sub2 = HashTableDword(rotor2);
    sub3 = (sub3 << 1) | (sub3 >> 31);
    if (rotor0 < 0)
        rotor0 += 0xF4;

    unsigned int sub1 = HashTableDword(rotor1);
    sub1 = (sub1 << 3) | (sub1 >> 29);
    int hiOut = (rotor3 << 8) | rotor2;
    sub2 = (sub2 << 2) | (sub2 >> 30);
    sub1 ^= sub2;
    sub1 ^= HashTableDword(rotor0);
    hiOut <<= 8;
    sub1 ^= sub3;
    hiOut |= rotor1;

    lo += sub1;

    hiOut <<= 8;
    hiOut |= rotor0;

    m_hi = hiOut;
    m_lo = lo;
    return lo;
}
