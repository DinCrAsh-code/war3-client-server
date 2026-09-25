//============================================================================
//  0x6F03B950 - SmartPositionTrack's vtable slot 1, the scalar deleting
//  destructor.
//
//  Own translation unit for the same reason positiontrackdeleteself.cpp is:
//  keeps the destructor call a real out-of-line `call` rather than
//  something this build could inline.
//
//  ~SmartPositionTrack() is implicit and trivial (no member of this class
//  has a destructor of its own), so - exactly like PositionTrack::DeleteSelf
//  (0x6F03A9F0, positiontrackdeleteself.cpp) - it collapses straight into
//  TRefCnt's own base destructor: the identical thirteen-instruction "reset
//  the vtable to TRefCnt's own, conditionally SMemFree" shape, at yet
//  another address, because every class in a TRefCnt chain needs its own
//  slot 1 to invoke the right derived destructor even when that destructor
//  does nothing beyond what the base's already does.
//============================================================================
#include "positiontrack.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

SmartPositionTrack* SmartPositionTrack::DeleteSelf(int flags)
{
    this->SmartPositionTrack::~SmartPositionTrack();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
