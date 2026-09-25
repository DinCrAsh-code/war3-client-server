//============================================================================
//  0x6F472DE0 - CAgentWar3's vtable slot 21 (+0x54), shared unchanged by
//  CWidget, CSelectable and CItem.  See agentwar3.h.
//
//  Slot 20's counterpart on the load side, and the more informative of the
//  two: read a record count, then for each record read an agent reference
//  and a dword and re-dispatch the pair through slot 2 (+0x08,
//  CObserver::Dispatch) with the message id the caller supplied.  So a
//  saved agent's message state is replayed as ordinary Dispatch traffic on
//  load, which is what makes slot 2's own three-argument shape observable
//  from a second direction.
//
//  A record whose agent reference did not resolve is skipped - read, but
//  not dispatched.
//
//  The agent reference is a real CAgentPtr (agent.h): the shipped tail is
//  the same test-decrement-release-if-last three instructions
//  CAgentPtr::Release has, and it runs once, after the loop, on whatever
//  the last iteration left in it.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  function in this family that builds a destructible object on the stack
//  (docs/msvc-vc8-idioms.md).
//
//  Own translation unit: four real calls plus the constructor.
//============================================================================
#include "agentwar3.h"
#include "cdatastore.h"

//  0x6F2D6790 - read one agent reference into an existing CAgentPtr.
//  Reconstructed in agentrefread.cpp; declared, not defined, here so this
//  call site reaches it with a real call.  __fastcall with the pointer's
//  address in edx, matching the shipped `lea edx,[esp+var_18]`; it also
//  hands the store back in eax, which this caller does not read.

CDataStore* __fastcall ReadAgentRef(CDataStore* store, CAgentPtr* out);

//  slot 2 (+0x08) - CObserver::Dispatch (observer_slots.cpp), reached
//  through the vtable: this is a base-class body and the six classes below
//  it are free to override it.
typedef void (__thiscall *Slot0x08Fn)(void* self, unsigned int msg,
                                      unsigned int arg1, void* arg2);

void CAgent::Method_0x54(CDataStore* store, unsigned int msgId, int)
{
    CAgentPtr agent(0);

    unsigned int count = 0;
    store->ReadDword(&count);

    for (unsigned int i = 0; i < count; i++)
    {
        ReadAgentRef(store, &agent);

        unsigned int value;
        store->ReadDword(&value);

        if (agent.m_ptr != 0)
            ((Slot0x08Fn)(*(void***)this)[0x08 / 4])(this, msgId, value,
                                                     agent.m_ptr);
    }
}
