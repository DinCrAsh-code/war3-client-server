//============================================================================
//  0x6F4D2000 - CConfigFile::GetFloatValue: *out = 0.0, then ValueAt() +
//  parse through Storm's own string-to-float ordinal.
//
//  Its own translation unit for the same reason src/configintvalue.cpp is.
//============================================================================
#include "config.h"

//  Storm.dll, imported by ordinal (574) - no published ordinal table found
//  for this one yet, so it keeps the dump's own descriptive name rather than
//  a guessed one, the same policy config.h already uses for Storm_590.  A
//  single string argument, result taken off the FPU stack (`fstp`) right
//  after the call with no stack cleanup visible at the call site - __stdcall,
//  one cdecl-shaped push, returning through ST(0) the way every x87 float
//  return does regardless of calling convention.
float __stdcall Storm_574(const char* s);

int __fastcall CConfigFile::GetFloatValue(const char* key, const char* section,
                                          CFloat* out, unsigned int index) const
{
    out->m_bits = 0;

    const char* value = ValueAt(key, section, index);
    if (value == 0)
        return 0;

    //  A genuine hardware-float store (`fstp dword ptr [edi]` in the shipped
    //  code, straight off Storm_574's own x87 return) - not a CFloat
    //  encode/decode, so the bit pattern is written directly rather than
    //  going through CFloatFromInt or an operator.
    *reinterpret_cast<float*>(&out->m_bits) = Storm_574(value);
    return 1;
}
