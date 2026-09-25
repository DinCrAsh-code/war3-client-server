//============================================================================
//  0x6F4A5E90 - LoadRelationLink: splice a freshly-loaded neighbour
//  reference in front of `slot` (one of the four list heads
//  NIpse::CRlAgent::Load reads): resolve the reference through LoadLinkRef
//  and, only for whichever of `slot`'s own two pointers actually got a
//  real value, write it - matching agentrelationsyncsave.cpp's own
//  SaveLinkRef exactly in reverse.
//
//  Own translation unit, deliberately: NIpse::CRlAgent::Load
//  (agentbaseabsload.cpp) calls this out of line, four times - same
//  reasoning as agentrelationsavelinkref.cpp's own header comment.
//============================================================================
#include "agentrelation.h"
#include "cdatastore.h"

SRelationLink* __fastcall LoadLinkRef(CDataStore* store);

void __fastcall LoadRelationLink(CDataStore* store, void* slot)
{
    SRelationLink* linkPtr = LoadLinkRef(store);

    void** slotWords = (void**)slot;
    if (linkPtr != 0)
        *(void**)linkPtr = slot;
    slotWords[1] = linkPtr;
}
