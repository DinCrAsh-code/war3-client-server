//============================================================================
//  0x6F71D070 - a monotonically increasing tick count, from Storm ordinal
//  406 called with three zeros.
//
//  Named for what its one caller in this closure does with it and nothing
//  more: the texture loader (0x6F732360) takes it on entry, takes it again
//  after a successful load, and accumulates the difference into
//  dword_6FADAB90.  That is the whole of the evidence - the ordinal is not
//  in any published Storm.dll export table, so the underlying call keeps its
//  ordinal name here for the reason config.h gives for Storm_590.
//============================================================================

//  Storm ordinal 406.  Three arguments, all zero at this one call site, and
//  it cleans them itself.
int __stdcall Storm_406(int a, int b, int c);

int GetStormTickCount()
{
    return Storm_406(0, 0, 0);
}
