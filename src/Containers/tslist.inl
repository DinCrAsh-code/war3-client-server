//============================================================================
//  TSExplicitList::UnlinkAll.  Kept out of storm.h so that a caller sees only
//  the declaration and has to emit a call, which is what the shipped code
//  does at every one of its four call sites.  Included by the translation
//  unit that owns the instantiation.
//============================================================================
#include "storm.h"
//----------------------------------------------------------------------------
//  0x6F46B560 - empty the list.
//
//  Written against m_prevlink rather than against Tail(): the shipped code
//  tests the raw field with `jle` and never materialises the masked pointer
//  Tail() would return, and it walks from the back because unlinking the tail
//  is the one operation the encoding makes cheap.
//----------------------------------------------------------------------------
template <class T, int LINKOFFSET>
void TSExplicitList<T, LINKOFFSET>::UnlinkAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        TSLink<T>* link = (TSLink<T>*)(m_linkoffset + tail);
        link->Unlink();
    }
}

