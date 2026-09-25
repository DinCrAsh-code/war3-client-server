//============================================================================
//  0x6F2F7830 - SAgentTimerArray::~SAgentTimerArray: destroy every live
//  element and give the block back to Storm.
//
//  It is TSGrowableArray's own destructor (storm.h / tsarray.inl) over a
//  0x14-byte element with a *virtual* destructor: the loop's
//  `mov edx,[eax+ebx]` / `mov eax,[edx+4]` / `push 0` / `call eax` is
//  `m_data[i].~T()` dispatched through slot 1, which is MSVC's scalar
//  deleting destructor with its flag word zero.  0x14 is CAgentTimer's own
//  size (agenttimer.h), and the Storm tag the free carries is
//  "AVCAgentTimer", which is what names the array.
//
//  Written out here rather than instantiated from tsarray.inl because that
//  file hard-codes one type-descriptor string for the __FILE__ slot and
//  this instantiation passes a different one.
//
//  Own translation unit: the element destructor and the free are both real
//  calls.
//============================================================================
#include "storm.h"

//----------------------------------------------------------------------------
//  One element, seen through the only thing this destructor touches: slot 1
//  of its vtable, and its size.
//----------------------------------------------------------------------------
struct SAgentTimerElement
{
    //  slot 1 (+0x04) - the scalar deleting destructor; `retn 4`, its one
    //  argument the "also free the storage" flag, which is zero here
    //  because the storage belongs to the array.
    void Destroy()
    {
        typedef void (__thiscall *DestroyFn)(void*, int);
        ((DestroyFn)(*(void***)this)[0x04 / 4])(this, 0);
    }

    char m_reserved00[0x14];
};

struct SAgentTimerArray
{
    ~SAgentTimerArray();

    unsigned int         m_alloc;   // +0x00
    unsigned int         m_count;   // +0x04
    SAgentTimerElement*  m_data;    // +0x08
    unsigned int         m_chunk;   // +0x0C
};

SAgentTimerArray::~SAgentTimerArray()
{
    for (unsigned int i = 0; i < m_count; i++)
        m_data[i].Destroy();

    if (m_data != 0)
        SMemFree(m_data, "AVCAgentTimer", -2, 0);
}
