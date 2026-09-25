//============================================================================
//  0x6F534A00 - copy this stream's own fixed name field (+0x104) into the
//  caller's buffer, bounded by the size it names.
//
//  A member, not a `__fastcall` free function: the shipped body puts the
//  stream in ecx and *pushes both* other arguments (`retn 8`), where a
//  __fastcall free function would take the second in edx and clean four
//  bytes too few.
//============================================================================
#include "cdatastore.h"

void __stdcall Storm_501(char* dest, const char* src, unsigned int size);

struct SNamedDataStream
{
    void CopyName(char* dest, unsigned int size);   // 0x6F534A00
    char m_reserved000[0x104];
    char m_name[0x104];         // +0x104
};

void SNamedDataStream::CopyName(char* dest, unsigned int size)
{
    Storm_501(dest, m_name, size);
}
