//============================================================================
//  0x6F03ED80 - SOwnedQueueRef::Release.  See unitownedref.h.
//
//  Own translation unit, separate from its twin at 0x6F02C960: same module
//  rule as everywhere else here - the two are in different address
//  neighbourhoods (0x6F02Cxxx and 0x6F03Exxx), so they shipped from
//  different objects, and putting them together would let this compiler
//  fold one into the other and leave only one body.
//============================================================================
#include "unitownedref.h"

//  vtable slot 2 (+0x08) - "detach", no arguments.
typedef void (__thiscall *DetachFn)(void* self);
//  vtable slot 0 - TRefCnt's own destroy-on-zero.
typedef void (__thiscall *DestroyFn)(void* self);

void SOwnedQueueRef::Release()
{
    TRefCnt* p = m_p;
    if (p != 0)
    {
        ((DetachFn)(*(void***)p)[0x08 / 4])(p);
        //  Re-read rather than reuse `p`: the shipped code reloads the
        //  field here, which is what says the detach above is allowed to
        //  have cleared it.
        p = m_p;
        if (p != 0 && --p->m_refcount == 0)
            ((DestroyFn)(*(void***)p)[0])(p);
        m_p = 0;
    }
}
