//============================================================================
//  The four-character type id's hash.
//============================================================================
#include "agiletype.h"

//----------------------------------------------------------------------------
//  0x6F4C8520 - Storm's string hash over the id's bytes, low byte first,
//  stopping at the first zero byte.  Each byte is folded in as the difference
//  of two entries of a sixteen-entry table, one per nibble; the sixteen values
//  are data this call tree never reads, so the table is only declared.
//----------------------------------------------------------------------------
unsigned int AGILE_TYPE_ID::Hash() const
{
    unsigned int seed1 = 0x7FED7FED;
    unsigned int seed2 = 0xEEEEEEEE;

    unsigned int value = m_id;
    while (value != 0)
    {
        unsigned int ch = value & 0xFF;
        seed1 = (seed1 + seed2) ^ (g_agileHashTable[ch >> 4] -
                                   g_agileHashTable[ch & 0x0F]);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
        value >>= 8;
    }

    return seed1;
}
