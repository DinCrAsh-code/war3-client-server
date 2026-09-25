//============================================================================
//  0x6F03BA70 - ProjectilePositionTrack's vtable slot 1, the scalar deleting
//  destructor.  Byte-identical shape to PositionTrack::DeleteSelf
//  (0x6F03A9F0, positiontrackdeleteself.cpp) at a different address:
//  ~ProjectilePositionTrack() is just as trivial (no member of this class
//  has a destructor of its own), so it collapses into the same
//  "reset the vtable to TRefCnt's own, conditionally SMemFree" shape every
//  class in this TRefCnt chain gets for slot 1.
//
//  Own translation unit for the same reason positiontrackdeleteself.cpp is:
//  keeps the destructor call a real out-of-line `call`.
//============================================================================
#include "projectilepositiontrack.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

PositionTrack* ProjectilePositionTrack::DeleteSelf(int flags)
{
    this->ProjectilePositionTrack::~ProjectilePositionTrack();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
