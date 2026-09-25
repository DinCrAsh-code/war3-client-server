//============================================================================
//  0x6F2D6790 - read one saved agent reference back into an existing
//  CAgentPtr.
//
//  Was a `mov eax,<address> / jmp eax` redirect; forty-two instructions, and
//  every one of the three functions it calls is already reconstructed here
//  (CDataStore::ReadDwordAlt, LookupHandle, CAgentPtr::AssignChecked).
//
//  Two dwords come off the stream - a handle and a type tag, in that order -
//  and go straight into LookupHandle.  The object that comes back is only
//  accepted if it also carries the '+agl' agile handle tag at +0x0C
//  (agentregistry.h, kAgileHandleTag): the handle table's own type check
//  has already passed by then, and this is the second, finer one game.h's
//  CHandleObject::m_kindTag describes.  What is actually assigned is not
//  that object but its +0x54, the same field the item-handle chain reaches
//  through.
//
//  Anything that fails either test clears the pointer instead - the release
//  is written out inline rather than as a `CAgentPtr::Release()` call
//  because it also has to null the field, which that helper does not do.
//
//  The store is returned in eax on both exits (`mov eax,esi` twice).  Its
//  one caller (agent_slot21.cpp) does not read it, but declaring this
//  `void` costs both of them.
//
//  Own translation unit: CAgent::Method_0x54 reaches it with a real call,
//  and it makes four of its own.
//============================================================================
#include "agent.h"
#include "agentregistry.h"
#include "cdatastore.h"
#include "game.h"

CDataStore* __fastcall ReadAgentRef(CDataStore* store, CAgentPtr* out)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDwordAlt(&handle);
    store->ReadDwordAlt(&typeTag);

    SItemHandleObject* object =
        (SItemHandleObject*)LookupHandle(handle, (int)typeTag);

    //  The two blocks come out in the opposite order from the shipped code,
    //  which puts the release inline and the tag check plus the assign
    //  out of line after it.  Tried four spellings - if/else both ways
    //  round, a nested if with an early return, and a forward `goto` past
    //  the release - and this compiler places the assign block first in
    //  every one of them.  Block placement is not source-addressable here;
    //  the instructions and their order within each block are identical.
    if (object != 0 && object->m_kindTag == (int)kAgileHandleTag)
    {
        out->AssignChecked((CAgent*)object->m_field54);
    }
    else
    {
        //  Written out rather than as `out->Release()`: that helper
        //  (agent.h) drops the reference but does not null the field, and
        //  the shipped code does both.
        CAgent* held = out->m_ptr;
        if (held != 0)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
            out->m_ptr = 0;
        }
    }

    return store;
}
