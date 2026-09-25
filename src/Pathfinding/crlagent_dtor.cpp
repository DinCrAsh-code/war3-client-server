//============================================================================
//  0x6F4803B0 - NIpse::CRlAgent::~CRlAgent().  See crlagent.h for the
//  layout.  Same unreproducible `__CxxFrameHandler3` SEH-frame mismatch as
//  the constructor (crlagent_ctor.cpp) and docs/msvc-vc8-idioms.md's own
//  writeup - the body below matches the dump instruction for instruction
//  past the frame furniture.
//
//  Restamps CRlAgent's own vtable first (the standard MSVC destructor
//  defensive-restamp, so no virtual call during unwinding can reach a
//  derived override on a now-partially-destroyed object), tears down the
//  CDynTable<CPrRelation*> sub-object at +0x58, then restamps straight to
//  NTempest::CPresence's own vtable and returns - CPresenceTagged's and
//  CPrRelation's own destructor bodies are trivial enough that the compiler
//  folded them into this same function with no further stamp of their own
//  (this class's own ctor, by contrast, *did* get a distinct CPrRelation
//  vtable stamp - see crlagent_ctor.cpp; a ctor and a dtor for the same
//  chain are not obliged to inline the same way).
//============================================================================
#include "crlagent.h"
#include "cellbuffer.h"

namespace NIpse {

//  0x6F4B3770 - NTempest::CMemBlock's own polymorphic base-class destructor
//  (stamps ??_7CMemBlock@NTempest@@6B@, calls FreeBlock, restamps
//  ??_7CEntity@NTempest@@6B@), already given a naked, self-redirecting
//  wrapper as ReleaseCMemBlockBase(void*) in Pathfinding/cellbuffer.cpp -
//  corrected this session (see that file's own comment) from a
//  no-parameter declaration nothing had ever called for real to
//  `__fastcall(void*)`, matching the real shipped `this`-only thiscall
//  ABI, so this is now a plain, real call to a named symbol rather than a
//  raw-address function-pointer cast.

//  See crlagent_ctor.cpp's own StampVtable for why this goes through a
//  volatile store rather than a plain literal assignment: two of this
//  destructor's own stamps target the same field, and a plain
//  compile-time-constant assignment lets dead-store elimination collapse
//  them into whichever one lands last.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

CRlAgent::~CRlAgent()
{
    StampVtable(this, (void*)0x6F95207C);     // ??_7CRlAgent@NIpse@@6B@ (defensive restamp)

    ReleaseCMemBlockBase(&m_relatedAgents);

    StampVtable(this, (void*)0x6F951CA0);     // ??_7CPresence@NTempest@@6B@
}

}  // namespace NIpse
