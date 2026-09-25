//============================================================================
//  0x6F4434A0 - `retn 8`, `__thiscall`-ish: `self` (raw, uninitialised
//  storage to placement-construct into) in ecx, `(ptr, len)` on the stack.
//  Placement-constructs a temporary read-only CDataStore view over
//  (ptr, len) - the same "wrap one wire record for reading" field sequence
//  (m_field8=0, m_field0xC=-1, m_readPos=0) every other CDataStore wrapper
//  in this neighbourhood spells out longhand (CNetEventTypeDispatcher_
//  ConstructAndBroadcast, netevent_dispatch.cpp), as a callable helper
//  instead. Reached from CNetData_DispatchOneQueuedRecord's own type-0x21/
//  0x2F special cases (Net/pausecallchainthunks.cpp) to build a *second*,
//  short-lived CDataStore over the same record it already wrapped once
//  itself for the type-0x17 case.
//============================================================================
#include <new>
#include "cdatastore.h"

CDataStore* __fastcall ConstructRecordView_6F4434A0(void* self, void* /*unused_edx*/,
                                                     void* ptr, unsigned int len)
{
    CDataStore* store = new (self) CDataStore();
    store->m_field4   = ptr;
    store->m_field8   = 0;
    store->m_field0xC = -1;
    store->m_field10  = (int)len;
    store->m_readPos  = 0;
    return store;
}
