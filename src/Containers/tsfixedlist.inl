//============================================================================
//  TSList::UnlinkAll - the compile-time-link-offset twin of tslist.inl's
//  TSExplicitList::UnlinkAll.  Same reason for living outside storm.h: the
//  shipped code calls it rather than inlining it, so a caller has to see only
//  the declaration.  Included by the translation unit that owns the
//  instantiation.
//============================================================================
#include "storm.h"
//----------------------------------------------------------------------------
//  0x6F6DE1B0 (instantiated for TSList<OsNet::OUTPUT, 0>) - empty the list.
//
//  Identical in shape to the explicit-offset version, minus the one
//  instruction that made them different functions in the first place: with
//  LINKOFFSET folded to 0 the tail value *is* the link address, so the walk
//  dereferences it directly instead of adding m_linkoffset to it first.
//----------------------------------------------------------------------------
template <class T, int LINKOFFSET>
void TSList<T, LINKOFFSET>::UnlinkAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        TSLink<T>* link = (TSLink<T>*)(LINKOFFSET + tail);
        link->Unlink();
    }
}
