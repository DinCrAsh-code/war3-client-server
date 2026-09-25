//============================================================================
//  0x6F676F10 and 0x6F6745F0 - two small helpers Net::NetClient vtable
//  slot 1's own root (0x6F67A170, netclient_installsession.cpp, networking
//  wave 8) calls directly on its own stack-local objects.
//============================================================================
void* __stdcall SMemReAlloc(void* ptr, unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void  __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

//  Storm allocation tag - canonicalises to an unresolved data symbol
//  (SYM) on both sides regardless of its own exact text, the same way
//  every other tag string in this codebase does; the real string is not
//  established by anything this session read (IDA's own display name,
//  "aAugameresultNe", is a truncated, sanitised label derived from the
//  string's own bytes, not the literal content).
static const char kSlot1ArrayTag[] = "";

//----------------------------------------------------------------------------
//  0x6F676F10 - a 20-byte-element growable array's own "resize to exactly
//  `newCount` elements and replace the contents from `srcData`" op: it does
//  not grow-and-append, it reallocates for `newCount` and then
//  unconditionally copies `newCount` whole elements in from the caller's
//  own source buffer, discarding whatever the array held before. `this+0`
//  and `this+4` both end up holding `newCount` - the disassembly writes
//  it to `this+0` immediately (before the realloc can even be attempted)
//  and again to `this+4` at the very end regardless of which path was
//  taken; kept as two separate stores rather than "simplified" to one,
//  since the shipped code plainly makes two.
//----------------------------------------------------------------------------
struct FixedElementArray20Fields
{
    unsigned int m_requestedCount; // +0x00
    unsigned int m_count;          // +0x04
    void*        m_data;           // +0x08
};

struct FixedElementArray20Self
{
    void ResizeAndCopyFrom(unsigned int newCount, const void* srcData);
    void ReleaseData();
};

void FixedElementArray20Self::ResizeAndCopyFrom(unsigned int newCount, const void* srcData)
{
    FixedElementArray20Fields* self = (FixedElementArray20Fields*)this;

    void* oldPtr = self->m_data;
    self->m_requestedCount = newCount;

    if (oldPtr != 0 || newCount != 0)
    {
        void* newPtr = SMemReAlloc(oldPtr, newCount * 0x14, kSlot1ArrayTag, -2, 0);
        self->m_data = newPtr;

        if (newCount != 0)
        {
            //  The shipped code re-reads `self->m_data` every iteration
            //  through the field rather than caching the just-reallocated
            //  pointer in a register across the loop (only the source
            //  pointer is cached) - the compiler cannot prove the write
            //  through `dst` doesn't alias `self` itself, so it reloads.
            //  Reproduced here by reading `self->m_data` fresh each pass
            //  instead of hoisting it into a local before the loop, which
            //  is what makes the difference: a hoisted local compiles to a
            //  tighter, but non-matching, pointer-increment loop instead.
            const char* src = (const char*)srcData;
            unsigned int offset = 0;
            unsigned int n = newCount;
            do
            {
                char* dst = (char*)self->m_data + offset;
                *(unsigned int*)(dst + 0x00) = *(unsigned int*)(src + offset + 0x00);
                *(unsigned int*)(dst + 0x04) = *(unsigned int*)(src + offset + 0x04);
                *(unsigned int*)(dst + 0x08) = *(unsigned int*)(src + offset + 0x08);
                *(unsigned int*)(dst + 0x0C) = *(unsigned int*)(src + offset + 0x0C);
                *(unsigned int*)(dst + 0x10) = *(unsigned int*)(src + offset + 0x10);
                offset += 0x14;
                n -= 1;
            } while (n != 0);
        }
    }

    self->m_count = newCount;
}

//----------------------------------------------------------------------------
//  0x6F676C50 - FixedElementArray20's own cleanup: free `this+8` (the
//  backing buffer, if any) with the same allocation tag, then leave the
//  now-dangling pointer untouched (no null-out - the shipped body's own
//  last instruction after the SMemFree call is a bare `retn`, not a store).
//----------------------------------------------------------------------------
void FixedElementArray20Self::ReleaseData()
{
    FixedElementArray20Fields* self = (FixedElementArray20Fields*)this;
    if (self->m_data != 0)
        SMemFree(self->m_data, kSlot1ArrayTag, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F6745F0 - copy a byte-length-prefixed field into `this` (a small
//  inline buffer): store the length byte at `this+0`, then memcpy `length`
//  bytes from `src+1` into `this+1`. Returns `this`. `retn 4` (one stack
//  argument, `this` in `ecx`) proves a real __thiscall member, the same
//  proof LenPrefixedCopier::Copy's own header (netclient_notifyevent_0x25.cpp)
//  documents for its own sibling shape - modelled the same way rather than
//  as a free function, since __thiscall cannot be spelled on one.
//----------------------------------------------------------------------------
struct ByteLengthPrefixedFieldCopier
{
    void* Copy(const unsigned char* src);
};

void* ByteLengthPrefixedFieldCopier::Copy(const unsigned char* src)
{
    unsigned char length = *src;
    *(unsigned char*)this = length;
    memcpy((char*)this + 1, src + 1, length);
    return this;
}
