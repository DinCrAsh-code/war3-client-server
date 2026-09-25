//============================================================================
//  0x6F255DD0 - CAbility depth<=5 closure batch 1 (swept in via
//  sub_6F264C50, not itself CAbility-specific - a generic bit-twiddle
//  utility, likely a slot/bucket count normaliser).  `this` is ecx but
//  never read; the one real argument is the sole stack dword (`retn 4`).
//
//  Finds the largest power of two that is <= the input (the classic
//  `x &= x-1` "clear the lowest set bit" loop, repeated until a single
//  bit is left), floors that at 1, and caps the whole thing at 8 for any
//  input already >= 8 - i.e. clamps a requested count down to a small
//  power-of-two bucket count in [1, 8].
//============================================================================

int __fastcall RoundDownToPowerOfTwoCapped8(void* /*this, unused*/, int value)
{
    if (value >= 8)
        return 8;

    int x = value;
    while ((x & (x - 1)) != 0)
        x = x & (x - 1);

    if (x < 1)
        x = 1;
    return x;
}

//----------------------------------------------------------------------------
//  0x6F255D00 - the same helper capped at 0x40 instead of 8; a distinct
//  address in the shipped image (own module, one bucket-count family used
//  at a different growth ceiling), not a duplicate this repo folds away.
//----------------------------------------------------------------------------
int __fastcall RoundDownToPowerOfTwoCapped64(void* /*this, unused*/, int value)
{
    if (value >= 0x40)
        return 0x40;

    int x = value;
    while ((x & (x - 1)) != 0)
        x = x & (x - 1);

    if (x < 1)
        x = 1;
    return x;
}
