//============================================================================
//  0x6F03E620 - write a refcounted pointer's own {handle, type} pair into a
//  CDataStoreScratch, or {-1, -1} when the pointer is null.
//
//  __fastcall with the store in ecx and the *address* of the pointer field
//  in edx (`lea edx, [this+off]` at every call site, not the pointer value
//  itself) - the same "address of the field, not the field" shape
//  CUnit::Save (unit_save.cpp) reaches it with for m_pRef2C4.  A different
//  offset pair from the inline {-1,-1}-or-{[+8],[+0xC]} pattern the same
//  function uses for its other refcounted-pointer fields: this one reads
//  the resolved object's +0xC and +0x10, not +8 and +0xC - so it is a real,
//  separate function rather than a duplicate of that inline shape, and is
//  named for what it does rather than folded into it.
//
//  Own translation unit: outside CDataStoreScratch's own 0x6F4C2xxx writer
//  cluster (cdatastorescratch.h) and CWidget's software-float module
//  (cdatastore_writecfloat.cpp) alike - its own address neighbourhood, so
//  its own file.
//============================================================================
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteRefHandlePair(CDataStoreScratch* store,
                                                  void** pRef)
{
    void* ref = *pRef;
    if (ref != 0)
    {
        store->WriteDword(*(unsigned int*)((char*)ref + 0x0C));
        store->WriteDword(*(unsigned int*)((char*)ref + 0x10));
    }
    else
    {
        store->WriteDword((unsigned int)-1);
        store->WriteDword((unsigned int)-1);
    }
    return store;
}
