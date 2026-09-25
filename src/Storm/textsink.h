//============================================================================
//  The debug text sink the "dump this object's state" vtable slots write
//  to, and the two small value types that know how to print themselves into
//  it.  Named for the tags the shipped code prints: "[IntMini]",
//  "[FltMiniB]", "[RefMini]".
//
//  Every one of these is reached with `this` in ecx and the sink on the
//  stack (`retn 4`), so they are members rather than __fastcall free
//  functions - the sink would otherwise land in edx and the `ret` counts
//  would disagree.
//============================================================================
#ifndef TEXTSINK_H
#define TEXTSINK_H

//  0x6F4B73F0 - the variadic sink itself.  __cdecl: the caller cleans every
//  dword of a whole run of calls in one `add esp, N`.  Out of this
//  session's scope; redirected rather than reconstructed (textsink.cpp).
void __cdecl LogFormatted(void* sink, const char* fmt, ...);

//  0x6F4B76E0 - format one CFloat into a static string buffer, base 10,
//  three decimals.  Also out of scope, also redirected.
const char* __fastcall FormatCFloat(const class CFloat* value, int base,
                                     int precision);

//----------------------------------------------------------------------------
//  0x6F472EB0 - one plain dword, printed as "[IntMini] val[0x%X]".
//----------------------------------------------------------------------------
struct SIntMiniValue
{
    void DumpTo(void* sink);

    unsigned int m_value;
};

//----------------------------------------------------------------------------
//  0x6F478550 - a two-word pair, printed as "[RefMini] t[%u] b[%u]".  The
//  same two words SOptionalHandleRef carries (itemhandleresolve.h).  Which
//  word is which comes straight out of the push order: arguments go right
//  to left, the shipped code pushes +0x04 first, so +0x04 is the *last*
//  format argument (b) and +0x00 is the first (t).
//----------------------------------------------------------------------------
struct SRefMiniValue
{
    void DumpTo(void* sink);

    unsigned int m_t;   // +0x00
    unsigned int m_b;   // +0x04
};

#endif
