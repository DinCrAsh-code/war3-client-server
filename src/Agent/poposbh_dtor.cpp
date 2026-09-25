//============================================================================
//  0x6F48CF40 - NIpse::CPoPosBh::~CPoPosBh(). See poposbh.h for the
//  class-level header comment. Own translation unit, deliberately - kept
//  out of line from ScalarDeletingDestructor (poposbh_typename_dtor.cpp),
//  same "put a callee in its own TU" convention
//  Pathfinding/crlagent_dtor.cpp/crlagent_scalardtor.cpp already use for
//  the exact same shape one level up this chain, otherwise /Ob2 inlines
//  the whole body into ScalarDeletingDestructor and loses the real `call`
//  the dump has.
//
//  ~40 instructions unreproducible: this destructor has an
//  __except_handler4-shaped SEH frame (destructible-local idiom,
//  docs/msvc-vc8-idioms.md's own section) that this build's fixed
//  /GS- /EHs-c- cannot reach with the bundled compiler - the same
//  accepted gap tools/funcmap.py's own MakeAbilityAgent_* entries already
//  document for this exact shape. The two real instructions the frame
//  wraps (the +0x9C CMemBlock release, the CPresence vtable restamp) are
//  present and in order.
//============================================================================
#include "poposbh.h"
#include "cellbuffer.h"

namespace NIpse {

CPoPosBh::~CPoPosBh()
{
    ReleaseCMemBlockBase((char*)this + 0x9C);
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@
}

}  // namespace NIpse
