//============================================================================
//  TRefCnt's own two vtable slots, 0x6F00C060 and 0x6F00C080.
//
//  They bracket ~TRefCnt (0x6F00C070, defined inline in refcnt.h), so all
//  three are one module - the same 0x6F00Cxxx run agent_ctor.cpp and
//  agent_dtor.cpp point at from their own neighbourhood comments.  Neither
//  calls the other: slot 0 reaches slot 1 through the vtable, which is a
//  dispatch and not a call, so nothing here can be inlined into anything
//  else by sharing a translation unit.
//============================================================================
#include "refcnt.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F00C060 - slot 0.  Seven instructions: null-test `this`, then
//  dispatch slot 1 with flags = 1, i.e. "destroy and free me".
//
//  The dispatch is spelled through the vtable by hand rather than as
//  `DeleteSelf(1)` for the reason refcnt.h already records for
//  TRefCnt::Release: slot 1's declared return type is covariant, so every
//  derived class narrows it, and calling it by name from the base emits the
//  base's own signature.  Reaching slot 1 by raw offset is what keeps the
//  one indirect call the shipped code makes.
//
//  The null test is the source's, not the compiler's: nothing before it
//  touches `this`, so there is nothing to prove the pointer good and
//  nothing for /O2 to fold the branch into.
//----------------------------------------------------------------------------
typedef void (__thiscall *DeleteSelfFn)(void* self, int flags);

void TRefCnt::ReleaseSelf()
{
    if (this != 0)
        ((DeleteSelfFn)(*(void***)this)[1])(this, 1);
}

//----------------------------------------------------------------------------
//  0x6F00C080 - slot 1, the scalar deleting destructor, and the shortest
//  one in the chain: ~TRefCnt is empty, so the whole of it inlines to the
//  single `mov [esi], offset ??_7TRefCnt@@6B@` vtable stamp.
//
//  That store is also why this one has no `this != 0` test where CAgent's
//  and CObserver's do (agent_dtor.cpp, observer.cpp) - it writes through
//  `this` before the test would be reached, so the compiler already knows
//  the pointer is good.  Same source shape, different codegen.
//----------------------------------------------------------------------------
TRefCnt* TRefCnt::DeleteSelf(int flags)
{
    this->TRefCnt::~TRefCnt();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
