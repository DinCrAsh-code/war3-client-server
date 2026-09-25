//============================================================================
//  0x6F459160 - a plain three-stack-arg (`__cdecl`, `retn` with no operand)
//  memcpy wrapper that hands back `dst` itself rather than memcpy's own
//  return value.  No caller in this closure's own dump names what it
//  copies; the shape alone (identical to a hundred other "copy then return
//  dst" wrappers this binary has) is the whole claim.
//============================================================================
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int size);

void* __cdecl CopyBufferReturningDst(void* dst, const void* src, unsigned int size)
{
    memcpy(dst, src, size);
    return dst;
}
