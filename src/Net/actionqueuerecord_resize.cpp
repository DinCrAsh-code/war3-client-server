//============================================================================
//  0x6F469800 - resize a Storm-allocated buffer at self+8 (self+0 the
//  requested size, self+4 the previous capacity) to `newSize`: try
//  SMemReAlloc in place first; on failure, SMemAlloc a fresh block and
//  manually copy min(newSize, oldCapacity) bytes across one at a time
//  (reproducing the shipped code's own per-byte null-destination guard,
//  which only ever matters if the fresh block's own base address happens
//  to land at exactly `-index`, i.e. never in practice), then free the old
//  block.
//============================================================================

void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void* __stdcall SMemReAlloc(void* ptr, unsigned int amount, const char* logfilename, int logline, unsigned int flags);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

static const char aE_1[] = "e:\\...";   // aE_1 - IDA's own truncated display; full text not reached by this call tree

class ResizableBuffer
{
public:
    void ResizeBuffer(unsigned int newSize);
};

void ResizableBuffer::ResizeBuffer(unsigned int newSize)
{
    char* s = (char*)this;
    void* oldPtr = *(void**)(s + 8);
    *(unsigned int*)s = newSize;

    void* newPtr = SMemReAlloc(oldPtr, newSize, aE_1, -2, 0x10);
    *(void**)(s + 8) = newPtr;
    if (newPtr != 0)
        return;

    newPtr = SMemAlloc(newSize, aE_1, -2, (unsigned int)newPtr);
    *(void**)(s + 8) = newPtr;
    if (oldPtr == 0)
        return;

    unsigned int oldCapacity = *(unsigned int*)(s + 4);
    unsigned int copyLen = newSize;
    if (copyLen >= oldCapacity)
        copyLen = oldCapacity;

    for (unsigned int i = 0; i < copyLen; ++i)
    {
        char* dst = *(char**)(s + 8) + i;
        if (dst != 0)
            *dst = *((char*)oldPtr + i);
    }

    SMemFree(oldPtr, aE_1, -2, 0);
}
