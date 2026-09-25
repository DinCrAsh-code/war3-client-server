//============================================================================
//  0x6F473180 - build (or fail to build) an IntegerListener for one
//  AbilityEntry (playerwar3.h), bind it through the out-of-scope helper at
//  0x6F480D50 (integerlistenerbindthunk.cpp), take one more reference on
//  success, and hand the result back through an out-parameter.  The one
//  caller in scope is CPlayerWar3's own slot-30 root (playerwar3_slot30.cpp,
//  through two thin argument-forwarding wrappers -
//  playerwar3_buildintegerlistenerwrap.cpp and
//  genericintegerlistenerbuilder.cpp).
//
//  `receiver->vtbl[5]()` is called first (zero stack arguments - `this`
//  only) and its result is threaded, unread by this function itself, into
//  the bind call below; no dump in this session's scope names what that
//  slot does.  Storm_401 allocates 0x10 bytes tagged ".\\Integer.cpp"
//  line 0x2B (43); on success the new object's fields are zeroed/`-1`-
//  filled and its vtable stamped to IntegerListener's own
//  (integerlistener.h) before the bind call; on failure the listener stays
//  null and the bind call still runs (unconditionally) with a null
//  receiver.
//============================================================================
#include "playerwar3.h"
#include "integerlistener.h"
#include "storm.h"

typedef void* (__thiscall *AbilityEntrySlot5Fn)(void*);

void CPlayerWar3::AbilityEntry::BuildIntegerListener(
    void** outListener, int minusOne, int six, void* owner, int zero)
{
    AbilityEntrySlot5Fn fn = (AbilityEntrySlot5Fn)m_vtbl[5];
    void* slot5Result = fn(this);

    void* mem = SMemAlloc(0x10, ".\\Integer.cpp", 0x2B, 0);
    IntegerListener* listener;
    if (mem)
        listener = new (mem) IntegerListener();
    else
        listener = 0;

    ((IntegerListenerBindHelper*)listener)->Bind(
        slot5Result, this, six, owner, minusOne, zero);

    if (listener)
        ++listener->m_refcount;

    *outListener = listener;
}
