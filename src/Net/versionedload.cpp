//============================================================================
//  0x6F051F00 - LoadVersionedObject: read a sub-object out of the save
//  stream if the stream's version says it is there.
//
//  Three outcomes, and the third is the interesting one: in range with a
//  target, the target's own vtable slot 9 (+0x24) reads itself; in range
//  with no target, nothing happens; *past* the upper bound, a throwaway
//  CAgentTimer is constructed on the stack purely so its slot 9 can consume
//  the bytes, and then destructed.  That is how the loader keeps its stream
//  position aligned across a field a newer save no longer wants.
//
//  The scratch object is a CAgentTimer specifically - the constructor at
//  0x6F0418E0 and the destructor at 0x6F477E20 are that class's own
//  (agenttimer.cpp) - which is what makes this the timer-shaped
//  instantiation of the helper rather than a generic one.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  destructible local in this family (docs/msvc-vc8-idioms.md): the
//  scratch timer's destructor has to run if slot 9 throws.
//
//  Own translation unit: five real calls out of it.
//============================================================================
#include "cdatastore.h"
#include "agenttimer.h"
#include "versiongate.h"

//  Whatever the gate points at, reached through its own vtable slot 9.
typedef void (__thiscall *LoadSelfFn)(void* self, CDataStore* store);

CDataStore* __fastcall LoadVersionedObject(CDataStore* store,
                                           const SVersionGate* gate)
{
    //  Two separate GetVersion calls, not one hoisted local: the shipped
    //  code calls 0x6F534A40 twice, once per bound, and compares the two
    //  answers against different fields.
    int atLeastSince = (store->GetVersion() >= gate->m_since);
    int atMostUntil = (gate->m_until >= store->GetVersion());

    if (atLeastSince && atMostUntil && gate->m_target != 0)
    {
        ((LoadSelfFn)(*(void***)gate->m_target)[0x24 / 4])(gate->m_target, store);
        return store;
    }

    if (!atMostUntil)
    {
        CAgentTimer scratch;
        ((LoadSelfFn)(*(void***)&scratch)[0x24 / 4])(&scratch, store);
    }

    return store;
}
