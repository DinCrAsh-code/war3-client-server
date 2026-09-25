//============================================================================
//  0x6F486580 - NTempest::CLinkedList<NIpse::CPoSeparate>::ScalarDeletingDestructor,
//  vtable slot 2 of that class's own vtable (0x6F9522BC) - its one override
//  of CPoSeparate's own 13 slots (see poseparate.h's own header comment).
//  Byte-identical to CPoSeparate::ScalarDeletingDestructor
//  (poseparate_dtor.cpp) apart from its own address, at a separate
//  address in a separate module neighbourhood - this image's own
//  /OPT:ICF-less link keeping identical bodies at different addresses, the
//  same as every other byte-identical-triplet/quadruplet this whole
//  family already documents. Own translation unit since it lives far from
//  CPoSeparate's own 0x6F4843xx/0x6F4844xx/0x6F495xxx modules.
//============================================================================
#include "poseparate.h"
#include "storm.h"

namespace NIpse {

void* CLinkedList_CPoSeparate::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
