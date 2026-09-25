//============================================================================
//  TSList<T,0>::DestroyAll - the "drain and free" twin of tsfixedlist.inl's
//  UnlinkAll: same TailLink()-driven walk and the same Unlink() on each
//  node, but the node is also handed to SMemFree once it is off the list
//  (LINKOFFSET 0 means the node *is* its own TSLink, so the tail value is
//  already the pointer to free - nothing to add).  Included by the
//  translation unit that owns the instantiation, same reasoning as the
//  other TSList/TSExplicitList .inl files: the shipped code calls this out
//  of line at every site, so a caller only ever sees the declaration.
//============================================================================
#include "storm.h"
//  What the shipped code hands Storm in the __FILE__ slot for this node's
//  own allocation tag - one instantiation so far (0x6F66F230), so the tag is
//  a macro the including file sets before the include, same convention as
//  tsarray.inl's TSARRAY_TYPENAME.
#ifndef TSLIST_DESTROY_TYPENAME
#define TSLIST_DESTROY_TYPENAME ".?AUFileInfo@Net@@"
#endif
static const char s_destroyTypeName[] = TSLIST_DESTROY_TYPENAME;

//----------------------------------------------------------------------------
//  0x6F66F230 (instantiated for TSList<Net::FileInfo, 0>).
//----------------------------------------------------------------------------
template <class T, int LINKOFFSET>
void TSList<T, LINKOFFSET>::DestroyAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        TSLink<T>* link = (TSLink<T>*)(LINKOFFSET + tail);
        link->Unlink();
        SMemFree(link, s_destroyTypeName, -2, 0);
    }
}
