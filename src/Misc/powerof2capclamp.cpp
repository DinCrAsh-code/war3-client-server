//============================================================================
//  0x6F7AEFC0 / 0x6F6F7480 - a byte-identical pair (same 17-instruction
//  body, same two-branch shape) in two different subsystems: this build
//  does not fold identical helpers across translation units, the same
//  duplication Misc/citemclosure_depth10_gates.cpp's own file header
//  documents for several of its own pairs.
//
//  `__thiscall`, `retn 4`: `this` in `ecx` (only ever written, at the +0x0C
//  field, never read - so nothing else about the object is established),
//  one stack argument.  For an input of 0x15 (21) or more, stamps a fixed
//  cap of 21 into `this+0x0C` and returns 21.  Otherwise rounds the input
//  down to the nearest power of two (the classic `n &= n - 1` "clear the
//  lowest set bit" loop, repeated until only one bit is left), and returns
//  1 rather than 0 if the input itself was 0.
//============================================================================

struct SPowerOf2CapField
{
    unsigned int ClampToPowerOfTwoCap20(unsigned int value);

    char         m_reserved00[0x0C];
    unsigned int m_cappedValue;   // +0x0C
};

//  0x6F7AEFC0.
unsigned int SPowerOf2CapField::ClampToPowerOfTwoCap20(unsigned int value)
{
    if (value < 0x15)
    {
        while ((value & (value - 1)) != 0)
            value &= value - 1;

        if (value < 1)
            value = 1;

        return value;
    }

    m_cappedValue = 0x15;
    return 0x15;
}

//  0x6F6F7480 - byte-identical to the above; a second, distinctly-named
//  class rather than a thunk to it, since both addresses belong to this
//  batch and a C++ call to an out-of-line member is not guaranteed to
//  compile down to the bare instructions reconstructing it directly does.
struct SPowerOf2CapFieldB
{
    unsigned int ClampToPowerOfTwoCap20(unsigned int value);

    char         m_reserved00[0x0C];
    unsigned int m_cappedValue;   // +0x0C
};

//  0x6F6F7480.
unsigned int SPowerOf2CapFieldB::ClampToPowerOfTwoCap20(unsigned int value)
{
    if (value < 0x15)
    {
        while ((value & (value - 1)) != 0)
            value &= value - 1;

        if (value < 1)
            value = 1;

        return value;
    }

    m_cappedValue = 0x15;
    return 0x15;
}
