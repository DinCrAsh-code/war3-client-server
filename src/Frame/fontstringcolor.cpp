//============================================================================
//  0x6F60E740 - CSimpleFontString::SetColor.
//
//  The four bytes are *rotated*, not copied: the source's byte 3 lands at
//  +0x68, byte 2 at +0x6B, byte 1 at +0x6A and byte 0 at +0x69.  That is one
//  ARGB word being stored as A followed by BGR, and it is written out here
//  byte by byte because that is exactly what the shipped stream does - a
//  single dword store would be a different instruction and a different value.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFontString::SetColor(const unsigned int* color)
{
    const unsigned char* src = (const unsigned char*)color;
    unsigned char* dst = m_color;

    dst[0] = src[3];
    dst[3] = src[2];
    dst[2] = src[1];
    dst[1] = src[0];

    NotifyColorChanged();
}
