//============================================================================
//  0x6F051CD0 - CBullet's vtable slot 1, the scalar-deleting destructor.
//  No separate non-scalar ~CBullet survives anywhere in this dump's own
//  reachable closure: the real destructor body is fully inlined here, the
//  outermost level nothing further derived inlines it into.
//
//  Its own body is exactly the "destruct members in reverse declaration
//  order, then tail into the base destructor" shape every level of this
//  family already documents (positiontrack.h's own header comment on the
//  implicit-destructor-collapse, bulletbase_ctordtor.cpp's own real
//  destructor):
//
//    - m_floatMini (FloatMini, floatmini.h) has an explicit but trivial
//      virtual destructor; destructing it would restamp its own vtable
//      pointer back to FloatMini's own - a no-op store to the value
//      already there, which the compiler elides outright.  No trace of it
//      survives in the shipped bytes, and none is expected here either.
//    - m_projectilePosition (ProjectilePositionTrack, a real, RTTI-named
//      sub-object one level more derived than TRefCnt) has no destructor
//      of its own either, so its implicit one collapses to the single
//      "reset the vtable to TRefCnt's own" store every class in this
//      TRefCnt chain gets (see projectilepositiontrackdeleteself.cpp's own
//      header comment for the identical shape one level up, on
//      ProjectilePositionTrack's own slot 1).
//    - CBulletBase::~CBulletBase() (0x6F03E800, bulletbase_ctordtor.cpp)
//      runs last, with a real, out-of-line `call` - it does real work
//      (destructs m_timer, restamps further down, tail-calls
//      CAgent::~CAgent()), so it survives as its own address rather than
//      being inlined in turn.
//
//  Letting the compiler generate CBullet's own implicit destructor and
//  calling it explicitly reproduces this exactly - no hand-written body
//  needed, the same idiom projectilepositiontrackdeleteself.cpp already
//  uses one level down.
//============================================================================
#include "bullet.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CBullet* CBullet::DeleteSelf(int flags)
{
    this->CBullet::~CBullet();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
