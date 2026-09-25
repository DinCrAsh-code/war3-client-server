//============================================================================
//  0x6F65E210 - bump the 64-bit reference count at (self->0)[index] (an
//  array of 8-byte counters, self->4 its own bound), or SErrSetLastError
//  and return 0 if `index` is out of range.
//============================================================================

void __stdcall SErrSetLastError(unsigned int code);
unsigned __int64 __fastcall Interlocked64IncrementAt(volatile unsigned __int64*);

class RefCountArray
{
public:
    unsigned __int64 IncrementRefCountAt(unsigned int index);

    void*         m_base;   // +0x00
    unsigned int  m_count;  // +0x04
};

unsigned __int64 RefCountArray::IncrementRefCountAt(unsigned int index)
{
    if (index >= m_count)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    return Interlocked64IncrementAt((volatile unsigned __int64*)((char*)m_base + index * 8));
}
