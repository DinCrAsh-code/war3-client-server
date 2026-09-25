//============================================================================
//  0x6F001A30 - CAgentWar3::Save.  See agentwar3.h.
//
//  One call into whatever 0x6F472420 is, then a fixed sweep: slot 20
//  (+0x50) once for every message id from 0xD0000 to 0xD03FF inclusive,
//  each with the same literal 0x400 as its third argument.  The bound is a
//  literal in the shipped code, not a count read from anywhere.
//
//  Own translation unit: two real calls, one of them in a loop.
//============================================================================
#include "agentwar3.h"
#include "cdatastorescratch.h"

//  0x6F472420 - the save-side preamble CAgentWar3::Save runs first.  It is
//  CAgent's own slot 14 body; reconstructed in agent_save.cpp (it was a
//  redirect until this session), and declared there rather than here so
//  this call site reaches it with a real call.

//  slot 20 (+0x50), reached through the vtable rather than by name: this is
//  a base-class body and any of the six classes below it may override it.
typedef void (__thiscall *Slot0x50Fn)(void* self, CDataStoreScratch* store,
                                      unsigned int msgId, int count);

void CAgentWar3::Save(CDataStoreScratch* store)
{
    CAgent::Save(store);

    //  A counted loop over a literal range, written as the shipped
    //  `mov esi,0D0000h` / ... / `add esi,1` / `cmp esi,0D0400h` / `jb`
    //  rather than as a zero-based index plus a base - the compiler keeps
    //  the id itself in the register and compares it against the end.
    for (unsigned int msgId = 0xD0000; msgId < 0xD0400; msgId++)
        ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, msgId, 0x400);
}
