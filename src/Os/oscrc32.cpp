//============================================================================
//  0x6F6B2660 - CRC-32 over `data`, seeded with `crc` and pre/post inverted,
//  through the 256-entry table at 0x6F972308.
//
//  The eight-at-a-time body is the shipped code's own unrolling and is
//  written out as an unrolled loop here for the same reason the software
//  float's leaves are transcribed: MSVC will not choose this shape on its
//  own at /O2, and the byte-at-a-time tail is a separate loop in the
//  shipped stream too.
//============================================================================
#include "os.h"

extern "C" const unsigned int g_crc32Table[256];   // 0x6F972308

unsigned int __fastcall OsCrc32(unsigned int crc, const void* data,
                                unsigned int length)
{
    if (data == 0)
        return 0;

    const unsigned char* p = (const unsigned char*)data;
    unsigned int acc = ~crc;

    if (length >= 8)
    {
        unsigned int blocks = length >> 3;
        do
        {
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p++) & 0xFF];
            length -= 8;
        }
        while (--blocks);
    }

    while (length != 0)
    {
        acc = (acc >> 8) ^ g_crc32Table[(acc ^ *p) & 0xFF];
        p++;
        length--;
    }

    return ~acc;
}
