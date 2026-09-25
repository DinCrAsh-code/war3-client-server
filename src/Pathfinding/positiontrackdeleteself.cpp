//============================================================================
//  0x6F03A9F0 - PositionTrack's vtable slot 1, the scalar deleting
//  destructor.
//
//  Own translation unit for the same reason agent_deleteself.cpp/
//  observer_deleteself.cpp are: keeps the destructor call a real
//  out-of-line `call` rather than something this build could inline.
//
//  ~PositionTrack() is implicit and trivial (no member of this class has a
//  destructor of its own), so the compiler collapses it straight into
//  TRefCnt's own base destructor - the identical thirteen-instruction
//  "reset the vtable to TRefCnt's own, conditionally SMemFree" shape
//  TRefCnt::DeleteSelf itself has (Agent/refcnt_deleteself.cpp): same
//  source shape, different address, because a class in a TRefCnt chain
//  needs its own slot 1 to invoke the right derived destructor even when
//  that destructor turns out to do nothing beyond what the base's already
//  does.
//============================================================================
#include "positiontrack.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

PositionTrack* PositionTrack::DeleteSelf(int flags)
{
    this->PositionTrack::~PositionTrack();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
