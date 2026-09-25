//============================================================================
//  0x6F208470 - CPlayerWar3::AbilityEntry::Destruct(), the per-element
//  destructor CPlayerWar3::~CPlayerWar3 (playerwar3_dtor.cpp) reaches
//  through an `eh vector destructor iterator` over the 26-entry array at
//  +0x40 (playerwar3.h).
//
//  The whole body is one instruction: re-stamp the object's vtable pointer
//  to TRefCnt's own (`??_7TRefCnt@@6B@`) and return - refcnt.h's own
//  `TRefCnt::~TRefCnt(){}` is exactly this shape (see that header's own
//  comment on why it is a plain, non-virtual, inline `{}` body), so
//  AbilityEntry's own element type derives from TRefCnt with nothing of
//  its own left to tear down at this level: no member destructor call, no
//  refcount release, nothing else - the *whole* class hierarchy above
//  TRefCnt for this element type must be trivial too, or this function
//  would have more in it. Explicitly invoking `~TRefCnt()` on a
//  reinterpreted pointer (rather than declaring AbilityEntry to formally
//  derive from TRefCnt in this header) keeps AbilityEntry's own layout
//  exactly as playerwar3.h already declares it - a raw `m_vtbl` member
//  Method_0x24/Method_0x20 (already EXACT) index directly - without
//  disturbing either of those already-scored bodies.
//
//  Own translation unit: 0x6F208470 is nowhere near the 0x6F40xxxx/
//  0x6F41xxxx CPlayerWar3 module neighbourhood.
//============================================================================
#include "playerwar3.h"
#include "refcnt.h"

void CPlayerWar3::AbilityEntry::Destruct()
{
    reinterpret_cast<TRefCnt*>(this)->~TRefCnt();
}
