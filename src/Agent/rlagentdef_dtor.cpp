//============================================================================
//  0x6F488DD0 - NIpse::CRlAgentDef::~CRlAgentDef().  See rlagentdef.h for
//  the layout and the unreproducible-SEH-frame note (same shape
//  Pathfinding/crlagent_ctor.cpp/crlagent_dtor.cpp already document).
//
//  Own translation unit: this destructor carries a real SEH frame in the
//  shipped body, which docs/msvc-vc8-idioms.md's own placement rule says
//  MSVC will not inline - so calling it from the scalar deleting
//  destructors' own file (rlagentdef_scalardtor.cpp) stays a real
//  out-of-line call either way, but keeping the definition here matches
//  the established convention for every other real-call destructor in this
//  chain.
//============================================================================
#include "rlagentdef.h"
#include "crlagent.h"
#include "cellbuffer.h"

namespace NIpse {

//  See Pathfinding/crlagent_dtor.cpp's own StampVtable comment for why this
//  goes through a volatile store rather than a plain literal assignment:
//  this destructor stamps the same field twice.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

CRlAgentDef::~CRlAgentDef()
{
    StampVtable(this, (void*)0x6F952794);     // ??_7CRlAgentDef@NIpse@@6B@ (defensive restamp)

    ReleaseCMemBlockBase((char*)this + 0x108);

    ((NIpse::CRlAgent*)((char*)this + 0x50))->~CRlAgent();

    StampVtable(this, (void*)0x6F951CA0);     // ??_7CPresence@NTempest@@6B@
}

}  // namespace NIpse
