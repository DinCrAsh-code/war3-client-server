//============================================================================
//  0x6F47C2B0 - SPathRefArray::Load.  See widgetpathrefadjust.h.
//
//  SPathRefArray::Save's counterpart (pathref_save.cpp): read the element
//  count as a word, grow the array to hold it (rounding the request up to a
//  whole allocation chunk when it is above 1), read every element back, and
//  finish with the two CFloats at +0x10/+0x14.
//
//  The grow is only attempted when the new count is above both the current
//  count *and* the current capacity at +0x00, which is what identifies that
//  first word as the capacity rather than more padding.
//
//  Own translation unit: five real calls.
//============================================================================
#include "widgetpathrefadjust.h"
#include "cdatastore.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F47B8B0 / 0x6F47B900 - the array's own chunk-size query and its
//  reallocation.  ChunkSize is reconstructed in pathrefarraygrow.cpp and
//  only declared here; SetAlloc is still redirected - it is three Storm
//  allocator ordinals (Storm_401/403/405) and nothing else.
struct SPathRefArrayGrow
{
    unsigned int ChunkSize(unsigned int wanted);
    void SetAlloc(unsigned int count);
};

__declspec(naked) void SPathRefArrayGrow::SetAlloc(unsigned int)
{
    __asm
    {
        mov     eax, 06F47B900h
        jmp     eax
    }
}

//  0x6F4AF090 - read one element back.  Out of scope; redirected.  A
//  thiscall member of the store with no arguments beyond `this`.
struct SPathRefReader
{
    void* ReadEntry();
};

__declspec(naked) void* SPathRefReader::ReadEntry()
{
    __asm
    {
        mov     eax, 06F4AF090h
        jmp     eax
    }
}

void SPathRefArray::Load(CDataStore* store)
{
    unsigned short count = 0;
    store->ReadWord(&count);

    unsigned int wanted = count;
    if (wanted > m_count)
    {
        if (wanted > *(const unsigned int*)m_reserved00)
        {
            //  Round the request up to a whole chunk, but only when there
            //  is more than one element to hold - the shipped code takes
            //  the `<= 1` case straight to SetAlloc with the raw count.
            unsigned int alloc = wanted;
            if (wanted > 1)
            {
                unsigned int chunk = ((SPathRefArrayGrow*)this)->ChunkSize(wanted);
                unsigned int rem = wanted % chunk;
                if (rem != 0)
                    alloc = chunk - rem + wanted;
            }
            ((SPathRefArrayGrow*)this)->SetAlloc(alloc);
        }
    }
    m_count = wanted;

    //  The loop bound is re-read from the *word* on the stack every
    //  iteration and compared 16 bits wide (`cmp si, word ptr [...]`), not
    //  against the widened local - so the index is a `unsigned short` too.
    unsigned short i = 0;
    while (i < count)
    {
        m_pItems[i] = (SPathRefCounted*)((SPathRefReader*)store)->ReadEntry();
        i++;
    }

    CFloat* pair = &m_x;
    ReadCFloat(store, pair);
    ReadCFloat(store, pair + 1);
}
