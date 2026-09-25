//============================================================================
//  0x6F001A70 - CAgentWar3::Load.  See agentwar3.h.
//
//  CAgentWar3::Save' exact counterpart (agentwar3_savemessages.cpp): the same
//  preamble-then-sweep shape, the same literal 0xD0000..0xD03FF range and
//  the same literal 0x400 third argument, through slot 21 (+0x54) instead
//  of slot 20 and with its own preamble at 0x6F472C70.
//
//  Own translation unit: two real calls, one of them in a loop.
//============================================================================
#include "agentwar3.h"
#include "cdatastore.h"

//  0x6F472C70 - the load-side preamble CAgentWar3::Load runs first.  It is
//  CAgent's own slot 15 body; reconstructed in agent_load.cpp (it was a
//  redirect until this session), and declared there rather than here so
//  this call site reaches it with a real call.

typedef void (__thiscall *Slot0x54Fn)(void* self, CDataStore* store,
                                      unsigned int msgId, int count);

void CAgentWar3::Load(CDataStore* store)
{
    CAgent::Load(store);

    for (unsigned int msgId = 0xD0000; msgId < 0xD0400; msgId++)
        ((Slot0x54Fn)(*(void***)this)[0x54 / 4])(this, store, msgId, 0x400);
}
