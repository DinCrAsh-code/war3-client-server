//============================================================================
//  0x6F4A5E40 - LoadLinkRef.  Read back an owner path-ref and an isB flag
//  (SaveLinkRef's own written pair, agentrelationsavelinkref.cpp) and
//  resolve them to the real neighbour link pointer: `&owner->m_linkB` if
//  the flag is set, `&owner->m_linkA` otherwise; null if there was no
//  owner at all.
//
//  Own translation unit, deliberately: kept out of line the same way
//  SaveLinkRef is (agentrelationsavelinkref.cpp's own header comment) -
//  Agent/agentbaseabsload.cpp already calls this one by declaration only
//  and scores EXACT, which is what any other caller needs too.
//============================================================================
#include "agentrelation.h"
#include "cdatastore.h"

//  0x6F4AF090 - read one path-ref entry back.  Out of scope; redirected -
//  see pathref_load.cpp, which declares the identical local wrapper for
//  the same reason.
struct SPathRefReader
{
    void* ReadEntry();
};

SRelationLink* __fastcall LoadLinkRef(CDataStore* store)
{
    void* owner = ((SPathRefReader*)store)->ReadEntry();

    unsigned int isB;
    store->ReadDword(&isB);

    if (owner != 0)
    {
        if (isB != 0)
            return (SRelationLink*)((char*)owner + 0x34);
        return (SRelationLink*)((char*)owner + 0x24);
    }
    return 0;
}
