//============================================================================
//  0x6F313C50 - copy a small variable-length record: a leading dword, a
//  count dword, then that many payload dwords.  `retn 4`.
//
//  CItem vtable-closure batch 2.  Called from 0x6F314120 (this batch, a
//  large blend-state helper left `TODO` - see docs/targets/
//  CItem-callee-batch2.md) and from 0x6F6D0E70 (a different, unrelated
//  caller outside this batch), so it is a small generic leaf rather than a
//  method of either caller's own class - no header in this tree names a
//  struct this shape belongs to.
//
//  A `dest == src` guard skips the payload copy and the count store
//  entirely (only the leading dword, already unconditional above the
//  check in the shipped stream, gets written) - self-assignment is a
//  deliberate no-op, not an oversight.
//
//  Own translation unit: a real out-of-line call from both callers above.
//============================================================================

//  `this`-in-ecx with one stack argument is plain __thiscall, but this
//  compiler only accepts that keyword on an actual member function - so a
//  one-liner receiver struct stands in for the free function shape this
//  address's own dump has (`this`/ecx is `dest`, matched dump for dump - no
//  separate wrapper call, this member function *is* 0x6F313C50).
struct SVariantRecordDest
{
    void CopyFrom(const void* src);
};

void __thiscall SVariantRecordDest::CopyFrom(const void* src)
{
    unsigned int* d = (unsigned int*)this;
    const unsigned int* s = (const unsigned int*)src;

    d[0] = s[0];

    if (this != src)
    {
        unsigned int count = s[1];
        for (unsigned int i = 0; i < count; i++)
            d[2 + i] = s[2 + i];
        d[1] = count;
    }
}
