//============================================================================
//  0x6F39E5C0 - `retn 0`, `__fastcall(data in ecx, length in edx)`.  A
//  running "xor in, rotate left 3" hash: the bulk of the buffer is folded
//  in four bytes at a time (as a `dword`, not byte-by-byte), then any
//  0-3 trailing bytes one at a time.  Not one of the three key policies
//  in Containers/allochashtable.h (those take a pre-hashed `hashval`
//  argument) - this is presumably what computes that value in the first
//  place, but no caller in this closure reaches far enough to confirm it.
//============================================================================

static unsigned int RotateLeft3(unsigned int value)
{
    return (value << 3) | (value >> 29);
}

unsigned int __fastcall RotatingHash(const void* data, unsigned int length)
{
    const unsigned char* p = (const unsigned char*)data;
    unsigned int dwordCount = length >> 2;
    unsigned int byteCount = length & 3;
    unsigned int hash = 0;

    for (unsigned int i = 0; i < dwordCount; i++)
    {
        hash = RotateLeft3(*(const unsigned int*)p ^ hash);
        p += 4;
    }

    for (unsigned int i = 0; i < byteCount; i++)
    {
        hash = RotateLeft3((unsigned int)*p ^ hash);
        p += 1;
    }

    return hash;
}
