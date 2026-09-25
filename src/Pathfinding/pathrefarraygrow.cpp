//============================================================================
//  0x6F47B8B0 - SPathRefArray's allocation-chunk query.
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is sixteen instructions
//  of bit twiddling with no calls.  SPathRefArray::Load (pathref_load.cpp)
//  rounds a requested element count up to a whole multiple of the answer.
//
//  The answer is the largest power of two that is <= the request, clamped
//  to [1, 0x40].  The loop is the standard "strip the lowest set bit until
//  one is left": `w & (w - 1)` clears the lowest set bit, so repeating it
//  while the result is non-zero leaves exactly the highest set bit.
//
//  Written with the first `w & (w - 1)` computed *before* the loop test
//  rather than as a `do`/`while` or a `for`: the shipped code is a rotated
//  while loop (`lea ecx,[eax-1]` / `and ecx,eax` / `jz` ahead of the body,
//  and the same pair again as the back edge), which is what this spelling
//  gives and what a `do { } while` does not.
//
//  Own translation unit: SPathRefArray::Load reaches it with a real call.
//============================================================================

struct SPathRefArrayGrow
{
    unsigned int ChunkSize(unsigned int wanted);
};

unsigned int SPathRefArrayGrow::ChunkSize(unsigned int wanted)
{
    //  The clamp is written as the *else* of the whole body rather than as
    //  an early return: the shipped code branches forward over everything
    //  to a `mov eax,40h` that shares its `retn` with the ordinary exit.
    if (wanted < 0x40)
    {
        unsigned int stripped = wanted & (wanted - 1);
        while (stripped != 0)
        {
            wanted = stripped;
            stripped = wanted & (wanted - 1);
        }

        //  `wanted < 1` rather than `wanted == 0`: the shipped test is
        //  `cmp eax,1` / `jnb`, which is what the ordered comparison builds
        //  and what an equality test against zero does not.
        if (wanted < 1)
            return 1;
        return wanted;
    }
    return 0x40;
}
