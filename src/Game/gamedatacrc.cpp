//============================================================================
//  0x6F534470 - the CRC-32 of one buffer, seeded the way this module always
//  seeds it: with the CRC of nothing at all.
//
//  That first call really is `OsCrc32(0, 0, 0)`, a null pointer and a zero
//  length, which OsCrc32 answers 0 to - so the seed is 0 and the call is
//  dead weight the shipped code makes anyway.  Reproduced, because deleting
//  it costs four instructions and changes the stream.
//============================================================================
#include "os.h"

unsigned int __fastcall GameDataCrc32(const void* data, unsigned int length)
{
    unsigned int seed = OsCrc32(0, 0, 0);
    return OsCrc32(seed, data, length);
}
