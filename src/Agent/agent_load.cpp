//============================================================================
//  0x6F472C70 - CAgent's own vtable slot 15 (+0x3C), the load-side preamble
//  CAgentWar3::Load (agentwar3_loadmessages.cpp) runs before its 0x400-wide
//  message sweep, and CAgent::Save's exact counterpart (agent_save.cpp).
//  Was a `mov eax,<address> / jmp eax` redirect.
//
//  It reads back what Save wrote, and reading the two side by side is what
//  says what the save format is: a pair count, then that many (key,
//  registrations) pairs, each pair a key followed by its own registration
//  count and that many {agent reference, dword} records.  Each record that
//  still resolves to a live agent is re-registered through
//  SModifierSubject::AddValueListener (0x6F471A30, misc_handle_lookups.cpp)
//  with the key as the listener handle and the record's dword as the
//  message id; one that does not resolve is read and dropped, exactly as
//  CAgent::Method_0x54 (agent_slot21.cpp) drops its own.
//
//  One CAgentPtr serves every record - it is constructed once, before the
//  slot-19 call, and ReadAgentRef assigns into it in place - so the release
//  at the end runs on whatever the last record left in it.  That is the
//  same single-pointer-reused shape slot 21 has.
//
//  **This cannot reach 1.000**, for the reason agent_save.cpp's header
//  gives: the `__except_handler4`-shaped SEH frame this repo's fixed
//  `/GS- /EHs-c-` never emits, plus the four bytes it puts under every
//  `[esp+N]`.
//
//  Own translation unit: eight real calls.
//============================================================================
#include "agent.h"
#include "cdatastore.h"
#include "positionmodifier.h"

//  0x6F2D6790 - read one agent reference into an existing CAgentPtr.
//  Reconstructed in agentrefread.cpp; declared, not defined, here so this
//  call site reaches it with a real call.  __fastcall with the pointer's
//  address in edx, matching the shipped `lea edx,[esp+var_18]`.
CDataStore* __fastcall ReadAgentRef(CDataStore* store, CAgentPtr* out);

void CAgent::Load(CDataStore* store)
{
    //  Constructed before the slot-19 call, which is where the shipped code
    //  has it, and its zero is the same zero the outer index starts at -
    //  the shipped `xor ebp,ebp` / `push ebp` pair is one constant serving
    //  both.
    unsigned int i = 0;
    CAgentPtr agent(0);

    LoadFlags(store);

    unsigned int pairs;
    store->ReadDword(&pairs);

    for (; i < pairs; i++)
    {
        unsigned int key;
        store->ReadDword(&key);

        unsigned int records;
        store->ReadDword(&records);

        for (unsigned int j = 0; j < records; j++)
        {
            ReadAgentRef(store, &agent);

            unsigned int msgId;
            store->ReadDword(&msgId);

            if (agent.m_ptr != 0)
                ((SModifierSubject*)this)->AddValueListener(key, msgId,
                                                            agent.m_ptr);
        }
    }

    Method_0x54(store, 0x40190064, 0x400);
    Method_0x54(store, 0x40190065, 0x400);
    Method_0x54(store, 0x40190066, 0x400);
    Method_0x54(store, 0x40190067, 0x400);
    Method_0x54(store, 0x40190068, 0x400);
}
